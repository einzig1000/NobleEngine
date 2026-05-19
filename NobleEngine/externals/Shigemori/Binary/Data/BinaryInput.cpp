#include "BinaryInput.h"
#include <d3d12.h>

namespace {
	template<typename T>
	std::shared_ptr<Value<T>> ValueDeserialize(std::string name, std::istream& in) {
		auto val = std::make_shared<Value<T>>(T(), name);
		val->Deserialize(in);
		return val;
	}
}

std::shared_ptr<ValueBase> BinaryInput::ReadBinary(std::istream& in) {
	// 1. TypeID
	uint8_t typeID;
	in.read(reinterpret_cast<char*>(&typeID), sizeof(typeID));

	// 2. Name長 + Name文字列
	uint8_t nameLength;
	in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

	std::string name(nameLength, '\0');
	in.read(&name[0], nameLength);

	uint8_t stringLength;
	std::string stringData;

	// 3. 型ごとのValue生成
	switch (static_cast<TypeID>(typeID)) {
	case TypeID::Int:
	{
		return ValueDeserialize<int>(name, in);
	}
	case TypeID::Float:
	{
		return ValueDeserialize<float>(name, in);
	}
	case TypeID::String:
	{
		in.read(reinterpret_cast<char*>(&stringLength), sizeof(stringLength));

		stringData = std::string(stringLength, '\0');
		in.read(&stringData[0], stringLength);

		return std::make_unique<Value<std::string>>(stringData, name);
	}
	case TypeID::Bool:
	{
		return ValueDeserialize<bool>(name, in);
	}
	case TypeID::Double:
	{
		return ValueDeserialize<double>(name, in);
	}
	case TypeID::Vector:
	{
		//todo std::vectorは未実装。いつかやる。工夫すればできるので各自工夫して
		return {};
	}
	case TypeID::uint8_t:
	{
		return ValueDeserialize<uint8_t>(name, in);
	}
	case TypeID::uint16_t:
	{
		return ValueDeserialize<uint16_t>(name, in);
	}
	case TypeID::uint32_t:
	{
		return ValueDeserialize<uint32_t>(name, in);
	}
	case TypeID::Vector2:
	{
		return ValueDeserialize<Vector2>(name, in);
	}
	case TypeID::Vector3:
	{
		return ValueDeserialize<Vector3>(name, in);
	}
	case TypeID::Vector4:
	{
		return ValueDeserialize<Vector4>(name, in);
	}
	// 他の型も同様に追加
	default:
		assert(false && "Unknown TypeID in File");
		return nullptr;
	}
}
