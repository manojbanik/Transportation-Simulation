#pragma once
#include <string>
#include "glut.h"

#include "Vector3D.h"

using namespace std;

template <typename T> string tostr(const T& t) { ostringstream os; os<<t; return os.str(); }

void printString(string str);
void printLargeString(string str);

Vector3 computeRotatedVector(Vector3 input, float angle);