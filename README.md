# Caro Game

Đây là sản phẩm do mình hoàn thành và được nộp để làm đồ án môn Nhập môn lập trình, Đại học Khoa học Tự nhiên, Kì 1 2021-2022. Bạn nào có tham khảo code, vui lòng ghi giúp mình đường dẫn [https://github.com/Noboroto/CaroGame/](https://github.com/Noboroto/CaroGame/) vào mục tài liệu tham khảo. Mình xin cảm ơn

## Thông tin kỹ thuật

- Phần mềm hiện chỉ đang khả dụng trên hệ điều hành Windows, do sử dụng thư viện Windows.h

## Chú ý dành cho các bạn chạy vscode

- Các bạn thay đường dẫn `C:/msys64/ucrt64/bin` trong các file được liệt kê bên dưới thành đường dẫn đến thư mục compiler của các bạn để chạy được nhé. 
- **Chú ý:** dấu `\` trong đường dẫn copy từ File Explorer cần thay bằng `\\` hoặc `/`
- Các thư mục gồm
  - `.vscode\c_cpp_properties.json`
  - `.vscode\launch.json`
  - `.vscode\tasks.json`

## Compile command

`g++ -g -std=c++14 CaroGame_noSound.cpp -pipe -lwinmm -o CaroGame.exe`