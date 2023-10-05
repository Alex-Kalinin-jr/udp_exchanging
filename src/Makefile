SRCFILES=udp_reciever/*.cc udp_reciever/*.cpp udp_sender/*.cc udp_sender/*.cpp
HDRFILES=udp_reciever/*.h udp_reciever/*.hpp udp_sender/*.h udp_sender/*.hpp

all: build
	./sender_app/udp_sender &
	./reciever_app/udp_reciever

build: clean
	mkdir sender_app reciever_app
	cmake -B./sender_app -S./udp_sender
	make -C./sender_app
	cmake -B./reciever_app -S./udp_reciever
	make -C./reciever_app

clean:
	rm -rf sender_app reciever_app

stop_sender:
	pkill -f "udp_sender"

style:
	clang-format -i -style=Google $(SRCFILES) $(HDRFILES)
check:
	cppcheck --language=c++ --std=c++17 --enable=all --suppress=unusedFunction \
	--suppress=missingInclude --suppress=unusedStructMember $(SRCFILES) $(HDRFILES) main.cc