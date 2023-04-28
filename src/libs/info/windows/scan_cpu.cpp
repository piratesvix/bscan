/************************************************************************************
      
 *                 Copyright (C) 2021 - 2023, Barca, Inc. 
 
 *    Email: <opensource@barca.com>  GitHub: @BarcaWebCloud. 
 *    Project: BSCAN to scanner MotherBoards. CPU, Memory Ram, SO and more
 
 * This software is licensed as described in the file COPYING, which                    
 * you should have received as part of this distribution. The terms                     
 * are also available at https://project-barca.github.io/docs/copyright.html.           
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell             
 * copies of the Software, and permit persons to whom the Software is                   
 * furnished to do so, under the terms of the COPYING file.                             
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY            
 * KIND, either express or implied.                                                      
 *
 **************************************************************************************/
#include "platform.h"

#ifdef BSCAN_WINDOWS
#include <algorithm>
#include <string>
#include <vector>
#include "WMIwrapper.h"
#include "hwares/scan_cpu.h"
#include "hwares/scan_cpuid.h"

namespace bscan {

  int CPU::currentClockSpeed_kHz() {
    std::vector<int64_t> speed{};
    wmi::queryWMI("Win32_Processor", "CurrentClockSpeed", speed);
    if (speed.empty()) {
      return -1;
    }
    return speed[0];
  }

  std::string CPU::getVendor() {
    std::vector<const wchar_t*> vendor{};
    wmi::queryWMI("Win32_Processor", "Manufacturer", vendor);
    if (vendor.empty()) {
  #if defined(BSCAN_X86)
      std::string v;
      uint32_t regs[4]{0};
      cpuid::cpuid(0, 0, regs);
      v += std::string((const char*)&regs[1], 4);
      v += std::string((const char*)&regs[3], 4);
      v += std::string((const char*)&regs[2], 4);
      return v;
  #else
      return "<unknown>";
  #endif
    }
    std::wstring tmp(vendor[0]);
    return {tmp.begin(), tmp.end()};
    return "<unknown>";
  }

  std::string CPU::getModelName() {
    std::vector<const wchar_t*> vendor{};
    wmi::queryWMI("Win32_Processor", "Name", vendor);
    if (vendor.empty()) {
  #if defined(BSCAN_X86)
      std::string model;
      uint32_t regs[4]{};
      for (unsigned i = 0x80000002; i < 0x80000005; ++i) {
        cpuid::cpuid(i, 0, regs);
        for (auto c : std::string((const char*)&regs[0], 4)) {
          if (std::isalnum(c) || c == '(' || c == ')' || c == '@' || c == ' ' || c == '-' || c == '.') {
            model += c;
          }
        }
        for (auto c : std::string((const char*)&regs[1], 4)) {
          if (std::isalnum(c) || c == '(' || c == ')' || c == '@' || c == ' ' || c == '-' || c == '.') {
            model += c;
          }
        }
        for (auto c : std::string((const char*)&regs[2], 4)) {
          if (std::isalnum(c) || c == '(' || c == ')' || c == '@' || c == ' ' || c == '-' || c == '.') {
            model += c;
          }
        }
        for (auto c : std::string((const char*)&regs[3], 4)) {
          if (std::isalnum(c) || c == '(' || c == ')' || c == '@' || c == ' ' || c == '-' || c == '.') {
            model += c;
          }
        }
      }
      return model;
  #else
      return "<unknown>";
  #endif
    }
    std::wstring tmp(vendor[0]);
    return {tmp.begin(), tmp.end()};
  }

  int CPU::getNumPhysicalCores() {
    std::vector<int> cores{};
    wmi::queryWMI("Win32_Processor", "NumberOfCores", cores);
    if (cores.empty()) {
  #if defined(BSCAN_X86)
      uint32_t regs[4]{};
      std::string vendorId = getVendor();
      std::for_each(vendorId.begin(), vendorId.end(), [](char& in) { in = ::toupper(in); });
      cpuid::cpuid(0, 0, regs);
      uint32_t HFS = regs[0];
      if (vendorId.find("INTEL") != std::string::npos) {
        if (HFS >= 11) {
          for (int lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl) {
            uint32_t regs_2[4]{};
            cpuid::cpuid(0x0b, lvl, regs_2);
            uint32_t currLevel = (LVL_TYPE & regs_2[2]) >> 8;
            if (currLevel == 0x01) {
              int numCores = getNumLogicalCores() / static_cast<int>(LVL_CORES & regs_2[1]);
              if (numCores > 0) {
                return numCores;
              }
            }
          }
        } else {
          if (HFS >= 4) {
            uint32_t regs_3[4]{};
            cpuid::cpuid(4, 0, regs_3);
            int numCores = getNumLogicalCores() / static_cast<int>(1 + ((regs_3[0] >> 26) & 0x3f));
            if (numCores > 0) {
              return numCores;
            }
          }
        }
      } else if (vendorId.find("AMD") != std::string::npos) {
        if (HFS > 0) {
          uint32_t regs_4[4]{};
          cpuid::cpuid(0x80000000, 0, regs_4);
          if (regs_4[0] >= 8) {
            int numCores = 1 + (regs_4[2] & 0xff);
            return numCores;
          }
        }
      }
      return -1;
  #else
      return -1;
  #endif
    }
    return cores[0];
  }

  int CPU::getNumLogicalCores() {
    std::vector<int> cores{};
    wmi::queryWMI("Win32_Processor", "NumberOfLogicalProcessors", cores);
    if (cores.empty()) {
  #if defined(BSCAN_X86)
      std::string vendorId = getVendor();
      std::for_each(vendorId.begin(), vendorId.end(), [](char& in) { in = ::toupper(in); });
      uint32_t regs[4]{};
      cpuid::cpuid(0, 0, regs);
      uint32_t HFS = regs[0];
      if (vendorId.find("INTEL") != std::string::npos) {
        if (HFS >= 0xb) {
          for (int lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl) {
            uint32_t regs_2[4]{};
            cpuid::cpuid(0x0b, lvl, regs_2);
            uint32_t currLevel = (LVL_TYPE & regs_2[2]) >> 8;
            if (currLevel == 0x02) {
              return static_cast<int>(LVL_CORES & regs_2[1]);
            }
          }
        }
      } else if (vendorId.find("AMD") != std::string::npos) {
        if (HFS > 0) {
          cpuid::cpuid(1, 0, regs);
          return static_cast<int>(regs[1] >> 16) & 0xff;
        }
        return 1;
      }
      return -1;
  #else
      return -1;
  #endif
    }
    return cores[0];
  }

  int CPU::getMaxClockSpeed_kHz() {
    std::vector<int64_t> speed{};
    wmi::queryWMI("Win32_Processor", "MaxClockSpeed", speed);
    if (speed.empty()) {
      return -1;
    }
    return speed[0] * 1000;
  }

  int CPU::getRegularClockSpeed_kHz() {
    std::vector<int64_t> speed{};
    wmi::queryWMI("Win32_Processor", "CurrentClockSpeed", speed);
    if (speed.empty()) {
      return -1;
    }
    return speed[0] * 1000;
  }

  int CPU::getCacheSize_Bytes() {
    std::vector<int64_t> cacheSize{};
    wmi::queryWMI("Win32_Processor", "L3CacheSize", cacheSize);
    if (cacheSize.empty()) {
      return -1;
    }
    return cacheSize[0] * 1024;
  }

  std::optional<CPU> getCPU(uint8_t socket_id) { return {}; }

  Socket::Socket(uint8_t id) : _id(id) {
    auto cpu = getCPU(_id);
    if (cpu.has_value()) {
      _cpu = cpu.value();
    }
  }

  Socket::Socket(uint8_t id, const class CPU& cpu) : _id(id) { _cpu = cpu; }


  std::vector<Socket> getAllSockets() {
    std::vector<Socket> sockets;

    return {};
  }

};

#endif  // END BSCAN_WINDOWS