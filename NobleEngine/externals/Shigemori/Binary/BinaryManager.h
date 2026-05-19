#pragma once
#include <memory>
#include "Data/BinaryOutput.h"
#include "Data/BinaryInput.h"
#include <string>
#include <vector>

//std::vectorと、std::stringやstd::vectorを含むクラス以外の型に対応

/// <summary>
/// Assets/Binの中身をバイナリ形式で色々するクラス
/// </summary>
class BinaryManager {
public:

	BinaryManager();
	~BinaryManager();

	template<typename T>
	void RegistOutput(T value, std::string name = "") {
		values.push_back(std::make_shared<Value<T>>(value, name));
	};

	template<typename T>
	static T Reverse(std::shared_ptr<ValueBase> value) {
		return static_cast<Value<T>*>(value.get())->value;
	}

	/// <summary>
	/// 実行後RegistしたValueはflushします
	/// </summary>
	void Write(std::string fileName);

	/// <summary>
	/// 指定したファイルからValueを作成します
	/// </summary>
	/// <returns>新たに生成したValue配列</returns>
	std::vector<std::shared_ptr<ValueBase>> Read(std::string fileName);

private:

	std::unique_ptr<BinaryInput> input;
	std::unique_ptr<BinaryOutput> output;
	std::vector<std::shared_ptr<ValueBase>> values;

	inline static const std::string basePath = "resources/Binary/"; // データの保存先パス

};
