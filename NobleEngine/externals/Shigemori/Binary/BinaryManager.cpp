#include "BinaryManager.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

BinaryManager::BinaryManager() {
	input = std::make_unique<BinaryInput>();
	output = std::make_unique<BinaryOutput>();

	fs::create_directories(basePath);
}

BinaryManager::~BinaryManager() {
}

void BinaryManager::Write(std::string fileName) {

	std::ofstream file(basePath + fileName, std::ios::binary);

	if (!file.is_open()) {
		return;
	}

	for (const auto& v : values) {
		output->WriteBinary(file, v.get());
	}

	values.clear();

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {

	std::ifstream file(basePath + fileName, std::ios::binary);
	if (!file.is_open()) {
		return {};
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input->ReadBinary(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}
