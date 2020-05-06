#pragma once

#include <array>
#include <atomic>
#include <vector>

namespace administration {

	extern std::atomic<bool> flag;
	//Threadsafe ... hopefully
	extern std::array<std::atomic<float>, 784> uniformData;
	extern uint32_t currentSelectedActor;
	extern std::vector<char*> actorNames;

	constexpr char RESOURCE_FOLDER[] = "./Resources/";
	constexpr char ACTOR_APPENDIX[] = "_Actor";
	constexpr uint32_t ACTOR_APPENDIX_SIZE = sizeof(ACTOR_APPENDIX) / sizeof(*ACTOR_APPENDIX);

	void loadAdministration() noexcept;

	void administration() noexcept;

}