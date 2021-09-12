#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description="Generate Gamma Lookup Tables.")
parser.add_argument('gammaValues', metavar="FLOAT", type=float, nargs='+', help="One or more Gamma Values. Each one will generate one lookup table.")
parser.add_argument('--type',      metavar="STR", default="const uint16_t", help="Output type")
parser.add_argument('--prefix',    metavar="STR", default="gammaLut_",      help="The lookup table will be named \"<prefix><gammaValue>\". Decimal points are converted to character 'p'.")
parser.add_argument('--attribute', metavar="STR", default="",               help="Optional attribute for table.")
parser.add_argument('--max',       metavar="INT", default=1023,             help="Maximum value returned - for example 1023 for 10 bits of resolution. Can be overridden by predefining the \"<prefix>MAX\" macro.")

if __name__ == '__main__':
	args = parser.parse_args()
	maxConstExpr = args.prefix.upper() + "MAX"
	print("#ifndef", maxConstExpr)
	print("#\tdefine", maxConstExpr, args.max)
	print("#endif")
	for gamma in args.gammaValues:
		print()
		gammaDiv, gammaMod = divmod(gamma, 1)
		gammaDecimals = '{:0.3n}'.format(gammaMod)[2:]
		gammaStr = str(int(gammaDiv))
		if gammaDecimals:
			gammaStr += 'p' + gammaDecimals
		print(args.type,
		      args.prefix + gammaStr + "[]" + ("" if not args.attribute else " "+args.attribute),
		      "= {",
		      end = ''
		)
		for i in range(256):
			if i % 4 == 0:
				print("\n\t", end='')
			print('{:.6E}'.format((pow(float(i)/255.0, gamma)))+"*"+maxConstExpr+"+0.5"+", ", end='')
		print("\n};")
