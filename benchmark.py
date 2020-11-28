import os
comp_algos = ['huff','adahuff', 'arith','tun','lz77','lzw']
data = ['dnact','englishct','xmlct','SD1.txt','SD2.txt','SD3.txt','SD4.txt']
tun_best = [4,7,7,8,6,4,3]
datadir = './data/'
zipdir = './zip/'
for algo in comp_algos :
    for idx, dataset in enumerate(data) :
        datapath = datadir+ dataset
        if algo == 'tun' :
            for bits in range(3,9) :
                cmd ="./main "+algo+" "+datapath + " --num_bits " + str(bits)
                print("-----------------")
                print(cmd)
                os.system( cmd);
                if bits == tun_best[idx] : 
                    #gzip
                    #bzip
        else :
            cmd = "./main "+algo+" "+ datapath;
            print("-----------------")
            print(cmd)
            os.system(cmd);
            #gzip
            #bzip
