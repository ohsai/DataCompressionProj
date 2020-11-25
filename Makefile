# 
# Reference Huffman coding
# Copyright (c) Project Nayuki
# 
# https://www.nayuki.io/page/reference-huffman-coding
# https://github.com/nayuki/Reference-Huffman-coding
# 


CXXFLAGS += -std=c++11 -O1 -Wall -Wextra -fsanitize=undefined -Ilib -g


.SUFFIXES:

.SECONDARY:

.DEFAULT_GOAL = all
.PHONY: all clean


MAIN = main
LIB = lib

HUFFMAN = $(LIB)/huffman
HUFFMAN_OBJ = CanonicalCode.o CodeTree.o FrequencyTable.o HuffmanCoder.o HuffmanCompress.o HuffmanDecompress.o AdaptiveHuffmanCompress.o AdaptiveHuffmanDecompress.o

ARITHMETIC = $(LIB)/arithmetic
ARITHMETIC_OBJ = ArithmeticCoder.o BitIoStream.o FrequencyTable.o PpmModel.o ArithmeticCompress.o ArithmeticDecompress.o 

TUNSTALL_OBJ = $(LIB)/tunstall.o

LZ_OBJ = $(LIB)/lzw.o

GOLOMB_OBJ = $(LIB)/golomb.o

OBJDIR = obj
MAIN_OBJ = $(OBJDIR)/$(MAIN).o
DEPDIR = .deps

OBJS = $(addprefix $(ARITHMETIC)/, $(ARITHMETIC_OBJ)) $(addprefix $(HUFFMAN)/, $(HUFFMAN_OBJ)) $(TUNSTALL_OBJ) $(LZ_OBJ) $(GOLOMB_OBJ) $(MAIN_OBJ)
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

$(TUNSTALL_OBJ): $(TUNSTALL_OBJ:.o=.cpp) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LZ_OBJ): $(LZ_OBJ:.o=.cpp) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(GOLOMB_OBJ): $(GOLOMB_OBJ:.o=.cpp) 
	$(CXX) $(CXXFLAGS) -c -o $@ $< -lz -lm

$(MAIN_OBJ): $(MAIN).cpp $(DEPDIR)/timestamp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ -MMD -MF $(DEPDIR)/$*.d $<

$(DEPDIR)/timestamp:
	mkdir -p $(DEPDIR)
	touch $(DEPDIR)/timestamp

-include $(DEPDIR)/*.d
