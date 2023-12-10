//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2023 Lorenz Bucher - all rights reserved
//  https://github.com/Sidelobe/HyperBuffer

#pragma once

#include <catch2/catch.hpp>
#include <fstream>

namespace TestCommon
{

static inline bool fileExists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

/** [ABCABCABC] --> [AAABBBCCC] */
static std::vector<float> deinterleave(std::vector<float> interleavedVector, int numChannels)
{
    std::vector<float> deinterleaved(interleavedVector.size());
    const int numSamples = (int)interleavedVector.size() / numChannels;
    assert(numSamples != 0);
    for (int i=0; i<numSamples; ++i) {
        for (int c=0; c<numChannels; ++c) {
            deinterleaved[c*numSamples+i] = interleavedVector[i*numChannels+c];
        }
    }
    return deinterleaved;
}

/** [ABCABCABC] --> [AAABBBCCC] */
static std::vector<float> interleave(std::vector<float> inlineVector, int numChannels)
{
    std::vector<float> interleaved(inlineVector.size());
    const int numSamples = (int)inlineVector.size() / numChannels;
    assert(numSamples != 0);
    for (int i=0; i<numSamples; ++i) {
        for (int c=0; c<numChannels; ++c) {
            interleaved[i*numChannels+c] = inlineVector[c*numSamples+i];
        }
    }
    return interleaved;
}

using stl_size_type = typename std::vector<float>::size_type;
constexpr stl_size_type STL(int i) { return static_cast<stl_size_type>(i); }

static inline std::vector<float> createRandomVector(int length, int seed=0)
{
    std::vector<float> result(STL(length));
    std::mt19937 engine(static_cast<unsigned>(seed));
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = static_cast<float>(dist(engine));
    }
    return result;
}

static inline std::vector<int> createRandomVectorInt(int length, int seed=0)
{
    std::vector<int> result(STL(length));
    std::mt19937 engine(static_cast<unsigned>(seed));
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = static_cast<int>(1000*dist(engine));
    }
    return result;
}



} // namespace TestCommon
