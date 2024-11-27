#pragma once

#include <codecvt>

#include "Core/helpers.h"
#include "libs/inicpp.h"

// the conversion functor must live in the "ini" namespace
namespace ini
{
    /** Conversion functor to parse std::vectors from an ini field-
     * The generic template can be passed down to the vector. */
    template<typename T>
    struct Convert<std::vector<T>>
    {
        /** Decodes a std::vector from a string. */
        void decode(const std::string &value, std::vector<T> &result)
        {
            result.clear();

            // variable to store the decoded value of each element
            T decoded;
            // maintain a start and end pos within the string
            size_t startPos = 0;
            size_t endPos = 0;
            size_t cnt;

            while(endPos != std::string::npos)
            {
                if(endPos != 0)
                    startPos = endPos + 1;
                // search for the next comma as separator
                endPos = value.find(',', startPos);

                // if no comma was found use the rest of the string
                // as input
                if(endPos == std::string::npos)
                    cnt = value.size() - startPos;
                else
                    cnt = endPos - startPos;

                std::string tmp = value.substr(startPos, cnt);
                // use the conversion functor for the type contained in
                // the vector, so the vector can use any type that
                // is compatible with inifile-cpp
                Convert<T> conv;
                conv.decode(tmp, decoded);
                result.push_back(decoded);
            }
        }

        /** Encodes a std::vector to a string. */
        void encode(const std::vector<T> &value, std::string &result)
        {
            // variable to store the encoded element value
            std::string encoded;
            // string stream to build the result stream
            std::stringstream ss;
            for(size_t i = 0; i < value.size(); ++i)
            {
                // use the conversion functor for the type contained in
                // the vector, so the vector can use any type that
                // is compatible with inifile-cp
                Convert<T> conv;
                conv.encode(value[i], encoded);
                ss << encoded;

                // if this is not the last element add a comma as separator
                if(i != value.size() - 1)
                    ss << ',';
            }
            // store the created string in the result
            result = ss.str();
        }
    };

	template<>
	struct Convert<std::wstring> {
		void decode(const std::string& value, std::wstring& result) {
			const char* test = value.c_str();

			result = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(test);
		}

		void encode(const std::wstring& value, std::string& result) {
			result = std::string(value.begin(), value.end());
		}
	};
}

class IniManager : public Singleton<IniManager>{
public:
	~IniManager() {
		SaveFile();
	}


public:
	static ini::IniFile& LoadFile() { return Get().LoadFile_Impl(); }
	static void SaveFile() { Get().SafeFile_Impl(); }
	static ini::IniFile& GetFile() {
		return file;
	}

protected:
	ini::IniFile& LoadFile_Impl();
	void SafeFile_Impl();


private:
	static ini::IniFile file;
	static std::string fileName;
};
