import subprocess
CacheArch = ["alpha","arm","btcminer","intel","mips"]
CachePara = ["""./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --inclusive""",
"""./src/cache --icache=128:2:2 --dcache=128:4:2 --l2cache=256:8:10 --blocksize=64 --memspeed=100""",
"""./src/cache --icache=0:0:0 --dcache=0:0:0 --l2cache=8:1:50 --blocksize=128 --memspeed=100""",
"""./src/cache --icache=256:1:2 --dcache=256:1:2 --l2cache=512:8:10 --blocksize=64 --memspeed=100 --inclusive""",
"""./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --inclusive"""]

testFile = "insertsort"
tmpFilePath = "./insertsort.txt"
tracesPath = "./traces/" + testFile + ".bz2"
# Get traces
bzhandle = subprocess.run(["bunzip2","-kc",tracesPath],stdout=subprocess.PIPE)
traces = bzhandle.stdout
for i in range(5):
	arch = CacheArch[i]
	simulatorPara = CachePara[i]
	print("testing %s with arg %s" %(arch,simulatorPara))
	calchandle = subprocess.Popen(simulatorPara.split(" "),stdin=subprocess.PIPE,stdout=subprocess.PIPE)
	result = calchandle.communicate(input=traces)[0]
	with open(tmpFilePath,"wb+") as handle:
		handle.write(result)
	correctOutputPath = "./correctOutput/" + arch + "/" + testFile + ".txt"
	diffhandle = subprocess.run(["diff",tmpFilePath,correctOutputPath],stdout=subprocess.PIPE)
	diffOutput = diffhandle.stdout
	if(diffOutput!=""):
		print("Diff presented when testing %s with arg %s" %(arch,simulatorPara))
		print("--------------------DIFF Presented--------------------------------")
		print(diffOutput.decode("utf-8"))