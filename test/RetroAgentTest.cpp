/*
 * RleTest.cpp
 *
 *  Created on: Jul 22, 2016
 *      Author: nadav
 */

#include "../src/environment/RetroAgent.h"
#include "../src/environment/Serializer.hxx"
#include "../src/environment/Deserializer.hxx"
#include <algorithm>
#include <iterator>
#include <string>
#include <thread>
#include "gtest/gtest.h"

namespace {
using namespace rle;
// The fixture for testing class Foo.
class RetroAgentTest : public ::testing::Test {
 protected:

	RetroAgentTest() {
  }

	string corePath = "/home/nadav/DQN/Arcade-Learning-Environment-2.0/snes9x2010/snes9x2010_libretro.so";
	string romPath = "/home/nadav/DQN/roms/mortal_kombat.sfc";
};

static void initRetroAgent(RetroAgent* adapter, string corePath, string romPath){
	adapter->loadCore(corePath);
	adapter->loadRom(romPath);
	int numSteps = 50;

	for(int i=0; i< numSteps; i++){
		adapter->run();
	}
}


TEST_F(RetroAgentTest, serialization) {
	RetroAgent adapter;
	initRetroAgent(&adapter, corePath, romPath);

	std::vector<uint8_t> dataOld;
	std::vector<uint8_t> dataNew;
	dataOld.assign(adapter.getRamAddress(2), adapter.getRamAddress(2) + adapter.getRamSize());
	Serializer ser;
	adapter.serialize(ser);

	int numSteps = 500;
	for(int i = 0 ; i < numSteps ; i++){
		adapter.run();
	}

	std::string serString = ser.get_str();
	Deserializer des(ser.get_str());
	adapter.deserialize(des);
	dataNew.assign(adapter.getRamAddress(2), adapter.getRamAddress(2) + adapter.getRamSize());

	EXPECT_EQ(dataOld, dataNew);
}

TEST_F(RetroAgentTest, multiThreading) {
	int numThreads(8);
	RetroAgent adapter[numThreads];
	std::vector<std::thread> threads;
	for(int i(0); i < numThreads; ++i){
		threads.push_back(std::thread(initRetroAgent, &adapter[i], corePath, romPath));
	}
	for (auto& th : threads){
	    th.join();
	}
}


}
