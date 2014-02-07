MC = Metil/metil_comp -I. -IPrepArg/src -Idic -O3 -g3

all: dl_Metil dl_PrepArg
	${MC} -o ex_edf_follower -ne main.cpp

anto32_1_1:
	${MC} main.cpp --flat-field "/data/visitor/ma1876/id19/data_correl/anto32_1_1/anto32_1_1_ref_0000.edf" --pattern "/data/visitor/ma1876/id19/data_correl/anto32_1_1/anto32_1_10%1_radio.edf" --fc 50 --result-file "/data/visitor/ma1876/id19/data_correl/anto32_1_1/resultCorrel_anto32_1_1_.txt"
 
anto38_1b:
	${MC} main.cpp --flat-field "/data/visitor/ma1876/id19/data_correl/anto38_1b/anto38_1b_ref_0000.edf" --pattern "/data/visitor/ma1876/id19/data_correl/anto38_1b/anto38_1b0%1_radio.edf" --fc 50 --result-file "/data/visitor/ma1876/id19/data_correl/anto38_1b/resultCorrel_anto38_1b.txt"
 
anto30_1:
	${MC} main.cpp --flat-field "/data/visitor/ma1876/id19/data_correl/anto30_1/anto30_1_ref_0000.edf" --pattern "/data/visitor/ma1876/id19/data_correl/anto30_1/anto30_10%1_radio.edf" --fc 50 --result-file "/data/visitor/ma1876/id19/data_correl/anto38_1b/resultCorrel_anto30_1.txt"
 
	
	
	
dl_Metil:
	test -e Metil || git clone git://gitosis.lmt.ens-cachan.fr/Metil-test Metil
	make -C Metil
	
dl_PrepArg:
	test -e PrepArg || git clone git://github.com/hleclerc/PrepArg.git
