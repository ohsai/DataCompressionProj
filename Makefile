# 
# Reference Huffman coding
# Copyright (c) Project Nayuki
# 
# https://www.nayuki.io/page/reference-huffman-coding
# https://github.com/nayuki/Reference-Huffman-coding
# 


CXXFLAGS += -std=c++11 -O1 -Wall -Wextra -fsanitize=undefined -Ilib


.SUFFIXES:

.SECONDARY:

.DEFAULT_GOAL = all
.PHONY: all clean


MAIN = main_glb

HUFFMAN = $(LIB)/huffman
HUFFMAN_OBJ = CanonicalCode.o CodeTree.o FrequencyTable.o HuffmanCoder.o

ARITHMETIC = $(LIB)/arithmetic
ARITHMETIC_OBJ = ArithmeticCoder.o BitIoStream.o FrequencyTable.o PpmModel.o

TUNSTALL_OBJ = tunstall.o

LZ_OBJ = lz77_78_W.o

GOLOMB_OBJ = golomb.o

OBJDIR = obj
MAIN_OBJ = $(OBJDIR)/$(MAIN).o
DEPDIR = .deps

LIB = lib
OBJS = $(addprefix $(ARITHMETIC)/, $(ARITHMETIC_OBJ)) $(addprefix $(HUFFMAN)/, $(HUFFMAN_OBJ)) $(TUNSTALL_OBJ) $(LZ_OBJ) $(MAIN_OBJ)
all: $(MAIN)

clean:
	rm -f $(OBJS)
	rm -rf -- $(OBJDIR) $(MAIN) #$(MAIN:=.o) $(MAIN)
	rm -rf $(DEPDIR)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(ARITHMETIC)/%.o: $(ARITHMETIC)/%.cpp $(DEPDIR)/timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF $(DEPDIR)/$*.d $<

$(HUFFMAN)/%.o: $(HUFFMAN)/%.cpp $(DEPDIR)/timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF $(DEPDIR)/$*.d $<

$(TUNSTALL_OBJ): $(LIB)/$(TUNSTALL_OBJ:.o=.cpp) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LZ_OBJ): $(LIB)/$(LZ_OBJ:.o=.cpp) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(GOLOMB_OBJ): $(LIB)/$(GOLOMB_OBJ:.o=.c) 
	$(CXX) $(CXXFLAGS) -c -o $@ $< -lz -lm

$(MAIN_OBJ): $(MAIN).cpp $(DEPDIR)/timestamp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF $(DEPDIR)/$*.d $<

$(DEPDIR)/timestamp:
	mkdir -p $(DEPDIR)
	touch $(DEPDIR)/timestamp

-include $(DEPDIR)/*.d
