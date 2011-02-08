#!/usr/bin/env python

import argparse
import sys
import collections
import re

class IdlObject(object):
    def __init__(self):
        self.doc = []

class Interface(IdlObject):
    def __init__(self):
        super(Interface, self).__init__()
        self.requirements = []
        self.definitions = []
        self.name = None

class Definition(IdlObject):
    def __init__(self):
        super(Definition, self).__init__()
        self.fields = []
        self.name = None
        self.id = None

class Field(IdlObject):
    def __init__(self):
        super(Field, self).__init__()
        self.name = None
        self.type = None

class ParseError(Exception):
    def __init__(self, msg):
        self._msg = msg
    def __str__(self):
        return self._msg

def CamelCase(string):
    string = string.lower()
    while string.find('_') >= 0:
        i = string.find('_')
        string = string[0:i] + string[i+1].upper() + string[i+2:]
    string = string[0].upper() + string[1:]
    return string

def tokenize(f):
    tokens = []
    for line in f:
        next = line
        while next:
            next = next.strip()
            if next.startswith('//'):
                token = next.rstrip()
                next = None
            else:
                bits = next.split(None, 1)
                if not bits:
                    break
                token = bits[0]
                next = None
                if len(bits) > 1:
                    next = bits[1]

            tokens.append(token)
        tokens.append('\n')
    return tokens

def parse(tokens):
    identifier = re.compile('[a-zA-Z_][a-zA-Z0-9_]*$')
    literal = re.compile('[a-zA-Z0-9_]*$')

    def isIgnored(token):
        return token.startswith('//') or token == '\n'

    def consumeToken(tokens):
        while tokens:
            t = tokens.popleft()
            if not isIgnored(t):
                return t
        return ''

    def consumeIgnored(tokens):
        t = tokens.popleft()
        return isIgnored(t)

    def consumeUntil(tokens, condition):
        consumed = []
        while tokens:
            t = tokens.popleft()
            if condition(t):
                return consumed
            consumed.append(t)
        return consumed

    t = collections.deque(tokens)
    interface = Interface()
    commentContext = interface

    while t:
        token = consumeToken(t)
        if token == '#require':
            requirement = t.popleft()
            if not identifier.match(requirement):
                raise ParseError('Expected a requirement')
            interface.requirements.append(requirement)
            if not consumeIgnored(t):
                raise ParseError('Expected new line after requirement')

        elif token == '#name':
            name = t.popleft()
            if not identifier.match(requirement):
                raise ParseError('Expected a name')
            interface.name = name
            if not consumeIgnored(t):
                raise ParseError('Expected new line after name')

        elif identifier.match(token):
            definition = Definition()
            definition.name = token
            interface.definitions.append(definition)
            commentContext = definition

            brace = consumeToken(t)
            if brace == '=':
                did = consumeToken(t)
                if literal.match(did):
                    definition.id = did
                else:
                    raise ParseError('Expected a definition id')
            elif brace != '{':
                raise ParseError('Expected an open brace')

            gotClose = False
            while t and not gotClose:
                token = consumeToken(t)
                if token == ';':
                    commentContext.doc.append(' '.join(consumeUntil(t, isIgnored)))
                if identifier.match(token):
                    field = Field()
                    field.name = token
                    definition.fields.append(field)
                    commentContext = field

                    colon = t.popleft()
                    field.type = t.popleft()
                    if colon != ':':
                        raise ParseError('Exected colon, got ' + colon)
                    if not identifier.match(field.type):
                        raise ParseError('Expected type, got ' + field.type)

                if token == '}':
                    gotClose = True
            if not gotClose:
                raise ParseError('Expected close brace')
        elif token:
            raise ParseError('Expected definition')
                    
    return interface

def outputHpp(interface):
    code = ['// Auto-generated by parse_idl.py\n'
            '#ifndef LM_AUTOGEN_{0}_HPP'.format(interface.name.upper()),
            '#define LM_AUTOGEN_{0}_HPP'.format(interface.name.upper()),
            '#include <string>\n',
            '#include "TypeWrapper.hpp"',
            '#include "UDPPacket.hpp"',
            '#include "PacketReader.hpp"',
            '#include "PacketHeader.hpp"',
            '#include "PacketWriter.hpp"\n']

    for req in interface.requirements:
        code.append('#include "{0}.hpp"'.format(req))

    code.append('\nnamespace LM {');

    code.append('\tenum {0}Enum {{'.format(interface.name))
    for item in interface.definitions:
        line = '\t\t{0}_{1}'.format(item.name, interface.name.upper())
        if item.id:
            line += ' = ' + item.id;
        line += ','
        code.append(line)
    code.append('\t};\n')

    code.append('\tclass {0}Receiver;\n'.format(interface.name))

    code.append('\tstruct {0} {{'.format(interface.name))
    code.append('\t\t{0}() {{ clear(); }}'.format(interface.name))
    code.append('\t\t{0}({0}Enum type);'.format(interface.name))
    code.append('\t\t{0}(const {0}& other);'.format(interface.name))
    code.append('\t\t~{0}();'.format(interface.name))
    code.append('\t\tvoid clear();')
    code.append('\t\tvoid marshal();')
    code.append('\t\tvoid unmarshal();')
    code.append('\t\tvoid dispatch({0}Receiver* r);'.format(interface.name, interface.name.lower()))
    for item in interface.definitions:
        code.append('\t\tstruct {0} {{'.format(CamelCase(item.name)))
        for field in item.fields:
            if field.type == 'string':
                code.append('\t\t\tTypeWrapper<std::string> {0};'.format(field.name))
            elif field.type in interface.requirements:
                code.append('\t\t\tTypeWrapper<{0}> {1};'.format(field.type, field.name))
            else:
                code.append('\t\t\t{0} {1};'.format(field.type, field.name))
        code.append('\t\t};\n')
    code.append('\t\t{0}Enum type;'.format(interface.name))
    code.append('\t\tUDPPacket raw;')
    code.append('\t\tPacketHeader header;\n')
    code.append('\t\tunion {')
    for item in interface.definitions:
        code.append('\t\t\t{0} {1};'.format(CamelCase(item.name), item.name.lower()))
    code.append('\t\t};')
    code.append('\t};\n')
    code.append('\tclass {0}Receiver {{'.format(interface.name))
    code.append('\tpublic:')
    code.append('\t\tvirtual ~{0}Receiver() {{ }}'.format(interface.name))
    for item in interface.definitions:
        code.append('\t\tvirtual void {0}(const Packet& p) {{ }}'.format(item.name.lower()))
    code.append('\t};\n')
    code.append('}\n')
    code.append('#endif')

    return code

def outputCpp(interface):
    code = ['// Auto-generated by parse_idl.py\n',
            '#include "{0}.hpp"'.format(interface.name),
            '#include <cstring>\n',
            'using namespace LM;',
            'using namespace std;\n']

    for item in interface.definitions:
        code.append('static void marshal_{1}(PacketWriter& w, {0}* p) {{'.format(interface.name, item.name))
        for field in item.fields:
            code.append('\tw << p->{0}.{1};'.format(item.name.lower(), field.name))
        code.append('}\n')

        code.append('static void unmarshal_{1}(PacketReader& r, {0}* p) {{'.format(interface.name, item.name))
        for field in item.fields:
            code.append('\tr >> p->{0}.{1};'.format(item.name.lower(), field.name))
        code.append('}\n')

    code.append('{0}::{0}({0}Enum type) {{'.format(interface.name))
    code.append('\tclear();');
    code.append('\tthis->type = type;');
    code.append('}\n')

    # This is necessary to ensure that copying happens properly
    code.append('{0}::{0}(const {0}& other) {{'.format(interface.name))
    code.append('\tclear();');
    code.append('\traw = other.raw;');
    code.append('\theader = other.header;');
    code.append('\ttype = other.type;');
    code.append('\tswitch(type) {')
    for item in interface.definitions:
        code.append('\tcase {1}_{0}:'.format(interface.name.upper(), item.name))
        for field in item.fields:
            if field.type == 'string' or field.type in interface.requirements:
                code.append('\t\t{0}.{1} = *other.{0}.{1};'.format(item.name.lower(), field.name))
            else:
                code.append('\t\t{0}.{1} = other.{0}.{1};'.format(item.name.lower(), field.name))
        code.append('\t\tbreak;\n')
    code.append('\t}')
    code.append('}\n')

    code.append('{0}::~{0}() {{'.format(interface.name))
    code.append('\tswitch(type) {')
    for item in interface.definitions:
        code.append('\tcase {1}_{0}:'.format(interface.name.upper(), item.name))
        for field in item.fields:
            if field.type == 'string' or field.type in interface.requirements:
                code.append('\t\tdelete {0}.{1}.item;'.format(item.name.lower(), field.name))
        code.append('\t\tbreak;\n')
    code.append('\t}')
    code.append('}\n')

    code.append('void {0}::clear() {{'.format(interface.name))
    code.append('\tvoid* base = &header;')
    code.append('\tmemset(base, 0, sizeof(*this) - (long(base) - long(this)));')
    code.append('}')

    code.append('void {0}::marshal() {{'.format(interface.name))
    code.append('\tPacketWriter w(type, header);')
    code.append('\tswitch(type) {')
    for item in interface.definitions:
        code.append('\tcase {1}_{0}:'.format(interface.name.upper(), item.name))
        code.append('\t\tmarshal_{0}(w, this);'.format(item.name))
        code.append('\t\tbreak;\n')
    code.append('\tdefault:');
    code.append('\t\tbreak;');
    code.append('\t}\n')
    code.append('\traw.fill(w.get_header().make_string());');
    code.append('\traw.append(w.packet_data());');
    code.append('}\n')

    code.append('void {0}::unmarshal() {{'.format(interface.name))
    code.append('\tPacketReader r(raw);')
    code.append('\ttype = ({0}Enum) r.packet_type();'.format(interface.name))
    code.append('\theader = r.get_header();')
    code.append('\tswitch(type) {')
    for item in interface.definitions:
        code.append('\tcase {1}_{0}:'.format(interface.name.upper(), item.name))
        code.append('\t\tunmarshal_{0}(r, this);'.format(item.name))
        code.append('\t\tbreak;\n')
    code.append('\tdefault:');
    code.append('\t\tbreak;');
    code.append('\t}')
    code.append('}')

    code.append('void {0}::dispatch({0}Receiver* r) {{'.format(interface.name))
    code.append('\tswitch(type) {')
    for item in interface.definitions:
        code.append('\tcase {1}_{0}:'.format(interface.name.upper(), item.name))
        code.append('\t\tr->{0}(*this);'.format(item.name.lower()))
        code.append('\t\tbreak;\n')
    code.append('\tdefault:');
    code.append('\t\tbreak;');
    code.append('\t}')
    code.append('}')
        
    return code

def outputWiki(interface):
    pass

if __name__ == "__main__":
    outputGens = {
        'cpp': outputCpp,
        'hpp': outputHpp,
        #'wikitext': outputWiki
    }

    parser = argparse.ArgumentParser(description="Parse the IDL for Leges Motus")
    
    parser.add_argument('-f', '--output-format', choices=['cpp', 'hpp', 'wikitext'],
                        nargs='?', help='the format to output', required=True)
    parser.add_argument('infile', type=argparse.FileType('r'), default=sys.stdin,
                        nargs='?', help='the IDL file to read from')
    parser.add_argument('outfile', type=argparse.FileType('w'), default=sys.stdout,
                        nargs='?', help='the file to write to')

    args = parser.parse_args()

    tokens = tokenize(args.infile)
    parsed = parse(tokens)
    output = outputGens[args.output_format](parsed)
    for line in output:
        args.outfile.write(line)
        args.outfile.write('\n')

    args.outfile.close()
