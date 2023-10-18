# $Id: Makefile,v 1.26 2023/10/06 11:12:37 leavens Exp leavens $
# Makefile for PL/0 lexer

# Add .exe to the end of target to get that suffix in the rules
LEXER = lexer
CC = gcc
LEX = flex
LEXFLAGS =
# on Linux, the following can be used with gcc:
# CFLAGS = -fsanitize=address -static-libasan -g -std=c17 -Wall
CFLAGS = -g -std=c17 -Wall
MV = mv
RM = rm -f
SUBMISSIONZIPFILE = submission.zip
ZIP = zip -9
PL0 = pl0
# Add the names of your own files with a .o suffix to link them into the VM
LEXER_OBJECTS = $(LEXER)_main.o $(LEXER).o $(PL0)_lexer.o \
		ast.o $(PL0).tab.o file_location.o utilities.o 

.DEFAULT: $(LEXER)

# create the lexer executable
$(LEXER) : $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: start-flex-file
start-flex-file:
	@if test -f $(PL0)_lexer.l; \
         then echo "$(PL0)_lexer.l already exists, not starting it!" >&2; \
              exit 2 ; \
         fi
	cat $(PL0)_lexer_definitions_top.l pl0_lexer_user_code.c > $(PL0)_lexer.l

# some special rules for generated files
$(PL0)_lexer.c: $(PL0)_lexer.l
	$(LEX) $(LEXFLAGS) $<

lexer.o: lexer.c lexer.h $(PL0).tab.h
	$(CC) $(CFLAGS) -c $<

$(PL0)_lexer.o: $(PL0)_lexer.c ast.h $(PL0).tab.h utilities.h file_location.h
	$(CC) $(CFLAGS) -Wno-unused-but-set-variable -Wno-unused-function -c $<

TESTS = hw2-test0.pl0 hw2-test1.pl0 hw2-test2.pl0 hw2-test3.pl0 \
	hw2-test4.pl0 hw2-test5.pl0 hw2-test6.pl0 hw2-test7.pl0

ERRTESTS = hw2-errtest1.pl0 hw2-errtest2.pl0 hw2-errtest3.pl0 \
	hw2-errtest4.pl0 hw2-errtest5.pl0
ALLTESTS = $(TESTS) $(ERRTESTS)
EXPECTEDOUTPUTS = $(ALLTESTS:.pl0=.out)
# STUDENTESTOUTPUTS is all of the .myo files corresponding to the tests
# if you add more tests, you can add more to this list,
# or just add to TESTS above
STUDENTTESTOUTPUTS = $(ALLTESTS:.pl0=.myo)

# rule for compiling individual .c files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) *~ '#'* *.stackdump core
	$(RM) *.o *.myo $(LEXER).exe $(LEXER)
	$(RM) $(SUBMISSIONZIPFILE)

# Rules for making individual outputs (e.g., execute make hw2-test1.myo)
# the .myo files are outputs from running the lexer on the .pl0 files
.PRECIOUS: %.myo
%.myo: %.pl0 $(LEXER)
	- ./$(LEXER) $< > $@ 2>&1

# main target for testing
.PHONY: check-outputs
check-outputs: $(LEXER) $(ALLTESTS)
	DIFFS=0; \
	for f in `echo $(ALLTESTS) | sed -e 's/\\.pl0//g'`; \
	do \
		echo running lexer on "$$f.pl0" ...; \
		./lexer "$$f.pl0" > "$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' \
			|| { echo 'failed!'; DIFFS=1; }; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All lexer tests passed!'; \
	else \
		echo 'Some lexer test(s) failed!'; \
	fi

# Automatically generate the submission zip file
$(SUBMISSIONZIPFILE): *.c *.h $(STUDENTTESTOUTPUTS) Makefile 
	$(ZIP) $@ $^ pl0.y pl0_lexer.l $(EXPECTEDOUTPUTS) $(ALLTESTS)

# instructor's section below...

YACC = bison
# The -Wno-other flag on bison is to prevent some warning about
# grammar the students need to write in HW3
YACCFLAGS = -Wno-other --locations -d -v

.PRECIOUS: $(PL0).tab.h
$(PL0).tab.o: $(PL0).tab.c $(PL0).tab.h
	$(CC) $(CFLAGS) -c $<

$(PL0).tab.c $(PL0).tab.h: $(PL0).y ast.h parser_types.h machine_types.h 
	@echo 'ignore the (warning) lines from $(YACC) below that start with:      | '
	$(YACC) $(YACCFLAGS) $(PL0).y

.PRECIOUS: %.out
%.out: %.pl0 $(LEXER)
	@if test '$(IMTHEINSTRUCTOR)' != true ; \
	then \
		echo 'Students should NOT use the target $@,'; \
		echo 'as using this target ($@) will invalidate a test'; \
		exit 1; \
	fi
	$(RM) $@
	- ./$(LEXER) $< > $@ 2>&1

.PHONY: create-outputs
create-outputs: $(LEXER) $(ALLTESTS)
	@if test '$(IMTHEINSTRUCTOR)' != true ; \
	then \
		echo 'Students should use the target check-outputs,' ;\
		echo 'as using this target (create-outputs) will invalidate the tests!' ; \
		exit 1; \
	fi
	for f in `echo $(ALLTESTS) | sed -e 's/\\.pl0//g'`; \
	do \
		$(RM) "$$f.out"; \
		echo running lexer on "$$f.pl0" ; \
		./$(LEXER) "$$f.pl0" > "$$f.out" 2>&1 || true ; \
	done
	echo 'done creating expected lexer outputs!'

.PHONY: digest
digest digest.txt: $(EXPECTEDOUTPUTS)
	for f in $(ALLTESTS) ; \
        do cat $$f; echo " "; \
	cat `echo $$f | sed -e 's/\\.pl0/.out/'`; \
        echo " "; echo " "; \
        done >digest.txt

# don't use develop-clean unless you want to regenerate the expected outputs
# and $(PL0).tab.[ch]
.PHONY: develop-clean
develop-clean: clean
	$(RM) digest.txt
	$(RM) y.tab.h
	$(RM) $(PL0)_lexer.[ch] $(PL0).tab.[ch] $(PL0).output

outputs-clean: clean
	$(RM) $(EXPECTEDOUTPUTS)

TESTSZIPFILE = ~/temp/hw2-tests.zip
PROVIDEDFILES = Makefile pl0.y pl0.tab.[ch] parser_types.h \
		pl0_lexer_definitions_top.l pl0_lexer_user_code.c \
                ast.[ch] machine_types.[ch] \
		file_location.[ch] lexer.[ch] utilities.[ch] \
		$(ALLTESTS) $(EXPECTEDOUTPUTS)

.PHONY: zip
zip $(TESTSZIPFILE): $(LEXER)

zip $(TESTSZIPFILE): create-outputs

zip $(TESTSZIPFILE): Makefile $(ALLTESTS) $(EXPECTEDOUTPUTS)
	$(RM) $(TESTSZIPFILE)
	chmod u+w Makefile $(EXPECTEDOUTPUTS) $(ALLTESTS) $(PROVIDEDFILES)
	$(ZIP) $(TESTSZIPFILE) Makefile $(TESTS) $(PROVIDEDFILES) $(EXPECTEDOUTPUTS)

.PHONY: check-separately
check-separately:
	$(CC) $(CFLAGS) -c *.c
