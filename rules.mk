# Default target
all: release

# Build rules
%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<
$(TARGET): $(DEPS) $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

release: CFLAGS += -O3
release: LDFLAGS += -O3
release: $(TARGET)
debug: CFLAGS += -g
debug: $(TARGET)

# Rules for updating lexer and parser
generate: 
	$(LEX) -o src/Core/QueryParser/Generated/lexer.cpp src/Core/QueryParser/flex.ll 
	$(YACC) -d -o src/Core/QueryParser/Generated/parser.cpp src/Core/QueryParser/grammar.yy 

# Clean rule
clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

.PHONY: all generate clean release debug
