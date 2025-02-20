build:
	g++ bank.cpp -o bank -I ~/vcpkg/installed/x64-linux/include
docker-build:
	docker build -t bank-app .
docker-run:
	docker run --rm -it -v ./data:/app/data bank-app
run:
	make docker-build && make docker-run