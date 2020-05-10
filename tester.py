#!/usr/bin/python3
# Some automated tests
# Every test case is passed as a command line argument
import pexpect

EXECUTABLE = "./mp"
TOLERANCE = 1e-6

# Literal-only tests
lit_tests = [
	["1+1", 2],
	["2+    2 * 2", 6],
	["12+1.1-5.6-8 + 9.22", 8.72],
	["123.456", 123.456],
	["2^0.5", 1.4142136],
	["(2 + 3) * (4 + 5)", 45],
	["(1- (1 /2)^(5-3))^0.5", 0.8660254]]

# Tests with predefined constants
const_tests = [
	["pi", 3.14159265],
	["e", 2.718281828],
	["pi^2 + e^0.5", 11.51832565],
	["pi-pi+e-e", 0]]

# returns true if a and b differ less than by TOLERANCE
def float_compare(a, b):
	return abs(a - b) < TOLERANCE

print("Performing literal tests:")
for i in range(len(lit_tests)):
	p = pexpect.spawn(EXECUTABLE + " \'" + lit_tests[i][0] + "\'")
	result = float(p.readline())

	if float_compare(lit_tests[i][1], result):
		print("Test no. " + str(i) + " passed.   ", end = "")
		print("(\'" + str(lit_tests[i][0]) + "\' --> " + str(result) + ")")
	else:
		print("Test no. " + str(i) + " failed!!! ", end = "")
		print("(\'" + str(lit_tests[i][0]) + "\' --> " + str(result) +
		      " [expected " + str(lit_tests[i][1]) + "])")

print("Performing constant tests:")
for i in range(len(const_tests)):
	p = pexpect.spawn(EXECUTABLE + " \'" + const_tests[i][0] + "\'")
	result = float(p.readline())

	if float_compare(const_tests[i][1], result):
		print("Test no. " + str(i) + " passed.   ", end = "")
		print("(\'" + str(const_tests[i][0]) + "\' --> " + str(result) + ")")
	else:
		print("Test no. " + str(i) + " failed!!! ", end = "")
		print("(\'" + str(const_tests[i][0]) + "\' --> " + str(result) +
		      " [expected " + str(const_tests[i][1]) + "])")
