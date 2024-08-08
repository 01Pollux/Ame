#include <iostream>
#include <string>
#include <span>
#include <vector>
#include <format>
#include <filesystem>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>

//

namespace bio = boost::iostreams;
namespace po  = boost::program_options;

std::vector<char> ReadFile(const std::string& inputName)
{
    std::ifstream file(inputName, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error(std::format("Failed to open file: {}", inputName));
    }

    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);

    return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::vector<char> CompressBlob(std::span<const char> data)
{
    std::vector<char> compressedData;

    bio::filtering_istream compressingStream;
    compressingStream.push(bio::gzip_compressor());
    compressingStream.push(bio::array_source(data.data(), data.size()));
    bio::copy(compressingStream, bio::back_inserter(compressedData));

    return compressedData;
}

void CompressAndWriteToFile(
    const std::string& input,
    const std::string& variableName,
    const std::string& outputName,
    const std::string& namespaceName)
{
    std::ofstream file(outputName, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error(std::format("Failed to open file: {}", outputName));
    }

    auto compressedData = CompressBlob(ReadFile(input));
    file << std::format("#pragma once\n\n");
    if (!namespaceName.empty())
    {
        file << std::format("namespace {}\n{{\n", namespaceName);
    }
    file << std::format("const unsigned char {}[] = {{\n", variableName);
    for (size_t i = 0; i < compressedData.size(); ++i)
    {
        file << std::format("0x{:02X}, ", static_cast<unsigned char>(compressedData[i]));
        if (i % 16 == 15)
        {
            file << "\n";
        }
    }
    file << "};\n";
    if (!namespaceName.empty())
    {
        file << "}\n";
    }
    file.close();

    std::cout << "File was successfully compressed and written to: " << outputName << std::endl;
}

// GZipCompressToC --input input.txt [--output output.txt] [--name MyVariable] [--namespace myNamespace]
int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help", "produce help message")
        ("input", po::value<std::string>(), "input file")
        ("output", po::value<std::string>(), "output file")
        ("name", po::value<std::string>(), "name of the file")
        ("namespace", po::value<std::string>(), "namespace of the file");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    std::string input;
    if (vm.count("input"))
    {
        input = vm["input"].as<std::string>();
    }
    else
    {
        std::cout << "Input file was not set.\n";
        return -1;
    }

    std::string variableName = vm.count("name") ? vm["name"].as<std::string>() : std::filesystem::path(input).stem().string();
    std::string output       = vm.count("output") ? vm["output"].as<std::string>() : std::format("{}.h", variableName);

    std::string namespaceName;
    if (vm.count("namespace"))
    {
        namespaceName = vm["namespace"].as<std::string>();
    }

    try
    {
        CompressAndWriteToFile(input, variableName, output, namespaceName);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}