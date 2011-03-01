add_library(lmbot
	ReactiveAIController.cpp
	AI.cpp
	FuzzyLogic.cpp
	FuzzyCategory.cpp
	FuzzyEnvironment.cpp
	AIController.cpp
	FuzzyLogicAI.cpp
)

add_executable(lmsimpleai simplemain.cpp)
add_executable(lmfuzzyai fuzzyaimain.cpp)

target_link_libraries(lmsimpleai lmbot lmclient lmcommon ${LIBS})
target_link_libraries(lmfuzzyai lmbot lmclient lmcommon ${LIBS})
