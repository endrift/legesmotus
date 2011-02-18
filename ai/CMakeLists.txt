add_library(lmbot
	ReactiveAIController.cpp
	StateTranslator.cpp
)

add_executable(lmreactiveai main.cpp)
target_link_libraries(lmreactiveai lmbot lmclient lmcommon Box2D)
