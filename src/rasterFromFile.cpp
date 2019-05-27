// Copyright (c) 2018  Robert J. Hijmans
//
// This file is part of the "spat" library.
//
// spat is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// spat is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with spat. If not, see <http://www.gnu.org/licenses/>.

#include "spatRaster.h"
#include "string_utils.h"
#include "readIniFile.h"


bool SpatRaster::constructFromFile(std::string fname) {

//	bool OK = (fname.substr(0, 3) == "HDF") || (file_exists(fname));
	bool OK = file_exists(fname);
	if (!OK) {
		setError("file does not exist");
		return false;
	}

	std::string ext = getFileExt(fname);

	if (ext != ".grd") {
        #ifdef useGDAL
		return constructFromFileGDAL(fname);
        #endif // useGDAL
	} else {
		std::vector<std::string> ini = readIni(fname);
		if (ini[15] == "false") {
			return false;
		}
		RasterSource s;
		double xmin = std::stod(ini[0]);
		double xmax = std::stod(ini[1]);
		double ymin = std::stod(ini[2]);
		double ymax = std::stod(ini[3]);
		SpatExtent e(xmin, xmax, ymin, ymax);
		s.extent = e;
		s.datatype = ini[4];
		s.bandorder = ini[5];
		s.byteorder = ini[6];

		s.nrow = std::stoi(ini[8]);
		s.ncol = std::stoi(ini[9]);
		s.nlyr = std::stoi(ini[10]);
		s.crs = ini[11];
		s.NAflag = std::stod(ini[12]);
		s.range_min = str2dbl(strsplit(ini[13], ":"));
		s.range_max = str2dbl(strsplit(ini[14], ":"));
		if (ini[7] == "1") {
			s.names = strsplit(ini[15], ":");
		} else {
			s.names = strsplit(ini[15], ":|:");				
		}
		s.filename = setFileExt(fname, ".gri");
		s.hasRange = std::vector<bool> (s.nlyr, true);
		s.has_scale_offset = std::vector<bool> (s.nlyr, false);
		s.scale = std::vector<double>(s.nlyr, 1);
		s.offset = std::vector<double>(s.nlyr, 0);

		s.hasValues = true;
		s.memory = false;
		s.driver = "raster";
		setSource(s);
	}
	return true;
}



bool SpatRaster::constructFromFiles(std::vector<std::string> fnames) {

	SpatRaster r = SpatRaster(fnames[0]);
	setSource(r.source[0]);
	for (size_t i=1; i<fnames.size(); i++) {
		r = SpatRaster(fnames[i]);
		if (!compare_geom(r, false, true, true)) {
			setError(fnames[i] = " does not match previous sources");
			return false;
		} else {
			addSource(r);
		}
	}
	return true;
}

