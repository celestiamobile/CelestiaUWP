#pragma once

#include "pch.h"
#include <string>

std::string Managed_Str_To_Std_Str(Platform::String^ ms);
Platform::String^ Std_Str_To_Managed_Str(const std::string& input);