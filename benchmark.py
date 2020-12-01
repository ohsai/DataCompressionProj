import os
comp_algos = ['huff','adahuff', 'arith','tun','lz77','lzw']
data = ['dnact','englishct','xmlct','SD1.txt','SD2.txt','SD3.txt','SD4.txt']
tun_best = [4,7,7,8,6,4,3]
datadir = './data/'
zipdir = './zip/'
os.system("rm -rf " + zipdir)
for algo in comp_algos :
    os.system("mkdir -p " + zipdir + algo + "/")
    for idx, dataset in enumerate(data) :
        datapath = datadir+ dataset
        encpath = datadir + dataset + "." + algo + ".enc"
        zippath = zipdir + algo + "/" + dataset + ".enc"
        if algo == 'tun' :
            for bits in range(3,9) :
                cmd ="./main "+algo+" "+datapath + " --num_bits " + str(bits)
                print("-----------------")
                print(cmd)
                os.system( cmd);
                if bits == tun_best[idx] : 
                    os.system("gzip -c " + encpath + " > " + zippath + ".gz")
                    os.system("bzip2 -c " + encpath + " > " + zippath + ".bz2")
                os.system("rm -f " + encpath)
        else :
            cmd = "./main "+algo+" "+ datapath;
            print("-----------------")
            print(cmd)
            os.system(cmd);
            os.system("gzip -c " + encpath + " > " + zippath + ".gz");
            os.system("bzip2 -c " + encpath + " > " + zippath + ".bz2");
            os.system("rm -f " + encpath)
