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
#ifdef BSCAN_APPLE
#include "hwares/scan_battery.h"

namespace bscan {

	std::string Battery::getVendor() const { return "<unknwon>"; }
	std::string Battery::getModel() const { return "<unknwon>"; }
	std::string Battery::getSerialNumber() const { return "<unknwon>"; }
	std::string Battery::getTechnology() const { return "<unknwon>"; }
	uint32_t Battery::getEnergyFull() const { return 0; }
	uint32_t Battery::energyNow() const { return 0; }
	bool Battery::charging() const { return false; }
	bool Battery::discharging() const { return false; }

	std::vector<Battery> getAllBatteries() {
	  return {};
	}

}

#endif // END BSCAN APPLE