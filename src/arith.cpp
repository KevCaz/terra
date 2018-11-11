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

#include <functional>
#include "spatraster.h"


template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::plus<T>());
    return result;
}


template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::minus<T>());
    return result;
}


template <typename T>
std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::divides<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::multiplies<T>());
    return result;
}


template <typename T>
std::vector<T> operator%(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
//    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::modulus<T>());
	for (size_t i=0; i<a.size(); i++) {
		result[i] = std::fmod(a[i], b[i]);
	}
    return result;
}


template <typename T>
std::vector<T> operator==(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::equal_to<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}

template <typename T>
std::vector<T> operator!=(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::not_equal_to<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}

template <typename T>
std::vector<T> operator>=(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::greater_equal<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}

template <typename T>
std::vector<T> operator<=(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::less_equal<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}


template <typename T>
std::vector<T> operator>(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::greater<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}

template <typename T>
std::vector<T> operator<(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::less<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			result[i] = NAN;
		} 
	}
    return result;
}


bool smooth_operator(std::string oper) {
	std::vector<std::string> f {"+", "-", "*", "/", "%", "==", "!=", ">", ",", ">=", "<="}; 
	return (std::find(f.begin(), f.end(), oper) != f.end());
}


SpatRaster SpatRaster::arith(SpatRaster x, std::string oper, std::string filename, std::string format, std::string datatype, bool overwrite) {
	
	SpatRaster out = geometry();

	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}
	
	if (!compare_geom(x, true, false)) {
		out.setError("dimensions and/or extent do not match");
		return(out);
	}
	
  	out.writeStart(filename, format, datatype, overwrite);
	readStart();
	x.readStart();
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> a = readBlock(out.bs, i);
		std::vector<double> b = x.readBlock(out.bs, i);
		if (oper == "+") {
			a = a + b; 
		} else if (oper == "-") {
			a = a - b; 
		} else if (oper == "*") {
			a = a * b; 
		} else if (oper == "/") {
			a = a / b; 
		} else if (oper == "%") {
			 a = a % b; 
		} else if (oper == "==") {
			a = a == b; 
		} else if (oper == "!=") {
			a = a == b; 
		} else if (oper == ">=") {
			a = a >= b; 
		} else if (oper == "<=") {
			a = a <= b; 
		} else if (oper == ">") {
			a = a > b; 
		} else if (oper == "<") {
			a = a < b; 
		} 
		out.writeValues(a, out.bs.row[i]);
	}
	out.writeStop();
	readStop();	
	x.readStop();	
	return(out);
}



SpatRaster SpatRaster::arith(double x, std::string oper, std::string filename, std::string format, std::string datatype, bool overwrite) {

	SpatRaster out = geometry();
	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}
	
  	out.writeStart(filename, format, datatype, overwrite);
	readStart();
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> a = readBlock(out.bs, i);
		if (std::isnan(x)) {
			for(double& d : a)  d = NAN;
		} else if (oper == "+") {
			for(double& d : a)  d += x;
		} else if (oper == "-") {
			for(double& d : a)  d -= x;
		} else if (oper == "*") {
			for(double& d : a)  d *= x;
		} else if (oper == "/") {
			for(double& d : a)  d /= x;
		} else if (oper == "%") {
			for(double& d : a) std::fmod(d,x);
		} else if (oper == "==") {
			for(double& d : a) if (!std::isnan(d)) d = d == x;
		} else if (oper == "!=") {
			for(double& d : a) if (!std::isnan(d)) d = d != x;
		} else if (oper == ">=") {
			for(double& d : a) if (!std::isnan(d)) d = d >= x;
		} else if (oper == "<=") {
			for(double& d : a) if (!std::isnan(d)) d = d <= x;
		} else if (oper == ">") {
			for(double& d : a) if (!std::isnan(d)) d = d > x;
		} else if (oper == "<") {
			for(double& d : a) if (!std::isnan(d)) d = d < x;
		} else {
			// stop
		}
		out.writeValues(a, out.bs.row[i]);
	}
	out.writeStop();
	readStop();		
	return(out);
}


SpatRaster SpatRaster::arith_rev(double x, std::string oper, std::string filename, std::string format, std::string datatype, bool overwrite) {

	SpatRaster out = geometry();
	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}
	
  	out.writeStart(filename, format, datatype, overwrite);
	readStart();
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> a = readBlock(out.bs, i);
		if (oper == "+") {
			for(double& d : a)  d = x + d;
		} else if (oper == "-") {
			for(double& d : a)  d = x - d;
		} else if (oper == "*") {
			for(double& d : a)  d = x * d;
		} else if (oper == "/") {
			for(double& d : a)  d = x / d;
		} else if (oper == "%") {
			for(double& d : a)  std::fmod(x,d);
		} else if (oper == "==") {
			for(double& d : a) d = d == x;
		} else if (oper == "!=") {
			for(double& d : a) d = d != x;
		} else if (oper == ">=") {
			for(double& d : a) d = x >= d;
		} else if (oper == "<=") {
			for(double& d : a) d = x <= d;			
		} else if (oper == ">") {
			for(double& d : a)  d = x > d;
		} else if (oper == "<") {
			for(double& d : a)  d = x < d;
		} else {
			// stop
		}
		out.writeValues(a, out.bs.row[i]);
	}
	out.writeStop();
	readStop();		
	return(out);
}


