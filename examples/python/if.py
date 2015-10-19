#!/usr/bin/python

def test(x):
	if x==0:
		y = 1
	else:
		y = 2
	assert(y>3)
	return y