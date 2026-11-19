#pragma once

#include <fstream>
#include <stdexcept>
#include <string>

void ReadBinaryFile(const std::string& filename);
void WriteBinaryFile(const std::string& filename, const std::string& text);
bool IsValidBmpFile(const std::string& filename);