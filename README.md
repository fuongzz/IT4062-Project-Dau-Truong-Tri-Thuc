# Đấu Trường Tri Thức

Đồ án cuối kỳ môn **Thực hành lập trình mạng (IT4062)**.

Đấu Trường Tri Thức là trò chơi trắc nghiệm nhiều người chơi qua mạng, hỗ trợ tối đa **100 người tham gia trong một cuộc chơi**. Người chơi cạnh tranh bằng kiến thức và tốc độ trả lời để trở thành người chơi chính, tích lũy điểm và giành chiến thắng.

## 1. Mục tiêu đề tài

- Xây dựng ứng dụng trò chơi theo mô hình client–server.
- Hỗ trợ nhiều người chơi kết nối và tham gia đồng thời.
- Trao đổi câu hỏi, câu trả lời và kết quả giữa máy chủ với các máy khách.
- Quản lý thời gian trả lời, vai trò, điểm số và trạng thái bị loại của từng người chơi.
- Đồng bộ diễn biến cuộc chơi cho tất cả người tham gia.

## 2. Quy định chung

- Người chơi phải đăng ký thông tin trước khi tham gia.
- Mỗi cuộc chơi có tối đa **100 người chơi**.
- Mỗi người chơi bắt đầu với **10 điểm**.
- Câu hỏi được đưa ra dưới hình thức **trắc nghiệm**.
- Trong lượt chơi chính thức, có một **người chơi chính**; những người còn lại là **người chơi phụ**.
- Người bị loại không tiếp tục trả lời trong cuộc chơi hiện tại.

## 3. Luật chơi

### 3.1. Chọn người chơi chính ban đầu

Khi bắt đầu cuộc chơi, tất cả người chơi nhận cùng một câu hỏi trắc nghiệm. Người **trả lời đúng nhanh nhất** được chọn làm người chơi chính; những người còn lại trở thành người chơi phụ.

### 3.2. Thời gian trả lời mỗi lượt

Tất cả người chơi nhận cùng một câu hỏi trắc nghiệm trong mỗi lượt.

| Vai trò | Thời gian suy nghĩ và trả lời |
| --- | --- |
| Người chơi phụ | 30 giây |
| Người chơi chính | 60 giây |

### 3.3. Người chơi chính trả lời đúng

- Người chơi chính nhận thêm số điểm bằng **tổng số điểm hiện có của tất cả người chơi phụ trả lời sai**.
- Những người chơi phụ trả lời sai bị loại khỏi cuộc chơi.
- Những người chơi phụ trả lời đúng tiếp tục tham gia.
- Nếu **không có người chơi phụ nào trả lời đúng**, người chơi chính chiến thắng.

### 3.4. Người chơi chính trả lời sai

- Người chơi chính bị loại khỏi cuộc chơi.
- Toàn bộ số điểm hiện có của người chơi chính được **chia đều cho những người chơi phụ còn lại**.
- Người chơi phụ **trả lời đúng nhanh nhất** trở thành người chơi chính ở lượt tiếp theo.
- Nếu **tất cả người chơi đều trả lời sai**, cuộc chơi kết thúc và **không có người chiến thắng**.

### 3.5. Người chơi chính bỏ qua câu hỏi

Người chơi chính có **2 lần được phép bỏ qua câu hỏi** để chuyển sang câu hỏi khác. Khi sử dụng quyền bỏ qua:

- Người chơi chính chia **một nửa số điểm hiện có** của mình cho những người chơi phụ đã trả lời đúng.
- Tổng số điểm hiện có của những người chơi phụ trả lời sai cũng được chia đều cho những người chơi phụ đã trả lời đúng.

### 3.6. Công thức tính điểm

Ký hiệu:

- `P`: điểm của người chơi chính trước khi xử lý kết quả lượt.
- `W`: tổng điểm trước khi xử lý kết quả lượt của những người chơi phụ trả lời sai.
- `C`: số người chơi phụ trả lời đúng.
- `R`: số người chơi phụ còn lại được nhận điểm khi người chơi chính trả lời sai.

| Tình huống | Cách tính |
| --- | --- |
| Người chơi chính trả lời đúng | Điểm mới của người chơi chính = `P + W` |
| Người chơi chính trả lời sai, có người nhận điểm | Mỗi người chơi phụ còn lại nhận thêm `P / R`, với `R > 0` |
| Người chơi chính bỏ qua, có người chơi phụ trả lời đúng | Người chơi chính còn `P / 2` điểm; mỗi người chơi phụ trả lời đúng nhận thêm `(P / 2 + W) / C`, với `C > 0` |

**Ví dụ:** Người chơi chính có 40 điểm và sử dụng quyền bỏ qua. Có 2 người chơi phụ trả lời đúng; tổng điểm của những người chơi phụ trả lời sai là 20. Người chơi chính còn 20 điểm, mỗi người chơi phụ trả lời đúng nhận thêm `(20 + 20) / 2 = 20` điểm.

## 4. Luồng cuộc chơi

1. Người chơi đăng ký thông tin và tham gia cuộc chơi.
2. Máy chủ gửi câu hỏi chọn người chơi chính cho tất cả người tham gia.
3. Máy chủ xác định người trả lời đúng nhanh nhất và thông báo vai trò.
4. Máy chủ gửi câu hỏi của lượt chơi, quản lý giới hạn 30 giây và 60 giây theo vai trò.
5. Máy chủ tiếp nhận câu trả lời hoặc yêu cầu bỏ qua của người chơi chính.
6. Máy chủ xử lý kết quả, cập nhật điểm, người bị loại và người chơi chính của lượt tiếp theo.
7. Kết quả được gửi đến các máy khách. Cuộc chơi tiếp tục hoặc kết thúc theo luật.

## 5. Chức năng dự kiến

### Máy chủ (server)

- Tiếp nhận và quản lý kết nối của tối đa 100 người chơi trong một cuộc chơi.
- Tiếp nhận thông tin đăng ký và quản lý danh sách người chơi.
- Quản lý câu hỏi trắc nghiệm và đáp án đúng.
- Điều phối các giai đoạn của cuộc chơi và thời gian trả lời.
- Xác định thứ tự trả lời đúng để chọn người chơi chính.
- Kiểm tra quyền bỏ qua, tính điểm và xử lý trạng thái bị loại.
- Gửi thông báo vai trò, câu hỏi, kết quả và kết thúc cuộc chơi đến các máy khách.

### Máy khách (client)

- Kết nối đến máy chủ và gửi thông tin đăng ký.
- Hiển thị câu hỏi, lựa chọn trả lời và thời gian còn lại.
- Gửi câu trả lời của người chơi đến máy chủ.
- Hiển thị vai trò, điểm số và trạng thái của người chơi.
- Cho phép người chơi chính sử dụng quyền bỏ qua và xem số lượt bỏ qua còn lại.
- Hiển thị kết quả từng lượt và kết quả cuối cuộc chơi.

## 6. Các quy ước cần thống nhất khi triển khai

Đề bài chưa quy định chi tiết các trường hợp dưới đây; nhóm cần thống nhất trước khi hiện thực để bảo đảm cách xử lý nhất quán:

- Hết thời gian mà chưa trả lời có được tính là trả lời sai hay không.
- Cách xác định người nhanh nhất, xử lý thời điểm trả lời trùng nhau và độ trễ mạng.
- Cách chọn người chơi chính nếu không ai trả lời đúng câu hỏi mở đầu.
- Khi người chơi chính trả lời sai, người chơi phụ trả lời sai có bị loại hay không và những ai được tính là “còn lại” để chia điểm.
- Khi bỏ qua câu hỏi, cách xử lý nếu không có người chơi phụ trả lời đúng và trạng thái của những người chơi phụ trả lời sai.
- Hai quyền bỏ qua được tính theo từng người chơi hay theo mỗi lần đảm nhận vai trò người chơi chính.
- Thời điểm được yêu cầu bỏ qua và cách xử lý câu trả lời đã gửi khi chuyển câu hỏi.
- Cách biểu diễn điểm lẻ, làm tròn và xử lý phần dư khi chia điểm.
- Cách xử lý khi người chơi mất kết nối hoặc rời cuộc chơi.

## 7. Trạng thái dự án

- Hiện chỉ README.md (10/11/2026)