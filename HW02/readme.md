Compile
gcc Server.c -o Server -lpthread
gcc Client.c -o Client

chạy ./Server
./Client 127.0.0.1 5500

---

Đăng nhập: USER username
Đăng bài: POST article
Đăng xuất: BYE
