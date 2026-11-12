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

Khi bắt đầu cuộc chơi, tất cả người chơi nhận cùng một câu hỏi trắc nghiệm. Người **trả lời đúng nhanh nhất** được chọn làm người chơi chính; những người còn lại trở thành người chơi phụ. Máy chủ xác định tốc độ trả lời theo thời điểm nhận được câu trả lời hợp lệ; nếu thời điểm trùng nhau, thứ tự máy chủ tiếp nhận được dùng để phân định.

Nếu không có ai trả lời đúng, máy chủ gửi một câu hỏi mở đầu mới và lặp lại vòng chọn người chơi chính cho đến khi có người trả lời đúng. Vòng chọn lại không làm thay đổi điểm và không loại người chơi vì trả lời sai hoặc hết giờ.

### 3.2. Thời gian trả lời mỗi lượt

Tất cả người chơi nhận cùng một câu hỏi trắc nghiệm trong mỗi lượt.

| Vai trò | Thời gian suy nghĩ và trả lời |
| --- | --- |
| Người chơi phụ | 30 giây |
| Người chơi chính | 60 giây |

Hết thời gian mà chưa trả lời được tính là trả lời sai. Máy chủ chốt kết quả lượt sau khi người chơi phụ hết 30 giây và người chơi chính đã trả lời, bỏ qua hoặc hết 60 giây. Nếu người chơi chính gửi câu trả lời hoặc yêu cầu bỏ qua sớm, máy chủ ghi nhận lựa chọn đó nhưng vẫn chờ hết thời gian của người chơi phụ để chốt kết quả.

### 3.3. Người chơi chính trả lời đúng

- Người chơi chính nhận thêm số điểm bằng **tổng số điểm hiện có của tất cả người chơi phụ trả lời sai**.
- Những người chơi phụ trả lời sai hoặc hết giờ bị loại khỏi cuộc chơi.
- Những người chơi phụ trả lời đúng tiếp tục tham gia.
- Nếu **không có người chơi phụ nào trả lời đúng**, người chơi chính chiến thắng.

### 3.4. Người chơi chính trả lời sai

- Người chơi chính bị loại khỏi cuộc chơi.
- Những người chơi phụ trả lời sai hoặc hết giờ cũng bị loại. Toàn bộ số điểm hiện có của người chơi chính được **chia đều cho những người chơi phụ trả lời đúng**.
- Người chơi phụ **trả lời đúng nhanh nhất** trở thành người chơi chính ở lượt tiếp theo.
- Nếu **không có người chơi phụ nào trả lời đúng**, cuộc chơi kết thúc và **không có người chiến thắng**.

### 3.5. Người chơi chính bỏ qua câu hỏi

Mỗi người chơi có tổng cộng **2 lần được phép bỏ qua trong một cuộc chơi**, chỉ sử dụng được khi đang là người chơi chính. Số lượt bỏ qua đã dùng được giữ nguyên qua các lần đảm nhận vai trò. Người chơi chính có thể gửi yêu cầu bỏ qua trong 60 giây của mình, máy chủ vẫn chờ hết 30 giây dành cho người phụ để chốt câu trả lời. Nếu người chính gửi yêu cầu bỏ qua sớm, máy chủ ghi nhận lựa chọn đó nhưng chỉ xử lý kết quả khi thời gian của người phụ kết thúc. Sau đó, câu hỏi mới bắt đầu một lượt thời gian mới. Với mỗi câu hỏi, lựa chọn hợp lệ đầu tiên của người chơi chính (trả lời hoặc bỏ qua) được ghi nhận; người chơi không thể đổi lựa chọn sau đó.

- Nếu có người chơi phụ trả lời đúng, người chơi chính giữ lại một nửa số điểm hiện có; nửa điểm còn lại cùng với tổng điểm của người chơi phụ trả lời sai được chia đều cho những người chơi phụ trả lời đúng. Người chơi phụ trả lời sai hoặc hết giờ bị loại.
- Nếu không có người chơi phụ nào trả lời đúng, không có điểm nào được chuyển. Người chơi chính giữ nguyên điểm và chiến thắng vì là người duy nhất còn lại; những người chơi phụ trả lời sai hoặc hết giờ bị loại.

### 3.6. Công thức tính điểm

Ký hiệu:

- `P`: điểm của người chơi chính trước khi xử lý kết quả lượt.
- `W`: tổng điểm trước khi xử lý kết quả lượt của những người chơi phụ trả lời sai hoặc hết giờ.
- `C`: số người chơi phụ trả lời đúng.
- `R`: số người chơi phụ trả lời đúng được nhận điểm khi người chơi chính trả lời sai.
- `H`: số điểm người chơi chính giữ lại khi bỏ qua, được tính bằng `floor(100 * P / 2) / 100`.

Trong công thức trên, `floor(x)` là lấy số nguyên xuống; vì điểm được tính theo đơn vị `0,01`, `H` được làm tròn xuống đến `0,01` điểm.

| Tình huống | Cách tính |
| --- | --- |
| Người chơi chính trả lời đúng | Điểm mới của người chơi chính = `P + W` |
| Người chơi chính trả lời sai, có người trả lời đúng | Toàn bộ `P` được chia đều cho `R` người chơi phụ trả lời đúng; phần dư `0,01` được phát theo thứ tự trả lời đúng nhanh nhất |
| Người chơi chính bỏ qua, có người chơi phụ trả lời đúng | Người chơi chính giữ lại `H`; quỹ chia là `P - H + W`, chia đều cho `C` người chơi phụ trả lời đúng; phần dư `0,01` được phát theo thứ tự trả lời đúng nhanh nhất |
| Người chơi chính bỏ qua, không có người chơi phụ trả lời đúng | Không chuyển điểm; người chơi chính giữ nguyên `P` và chiến thắng |

Điểm được lưu đến **2 chữ số thập phân** và tính theo đơn vị `0,01`. Khi chia điểm, phần dư được phát từng `0,01` cho những người nhận theo thứ tự trả lời đúng nhanh nhất. Khi bỏ qua, nếu `100 * P` là số lẻ, điểm người chơi chính giữ lại được làm tròn xuống đến `0,01`; phần còn lại được đưa vào quỹ chia.

**Ví dụ 1:** Người chơi chính có 40 điểm và sử dụng quyền bỏ qua. Có 2 người chơi phụ trả lời đúng; tổng điểm của những người chơi phụ trả lời sai là 20. Người chơi chính còn 20 điểm, mỗi người chơi phụ trả lời đúng nhận thêm `(20 + 20) / 2 = 20` điểm.

**Ví dụ 2:** 10 điểm chia cho 3 người chơi phụ trả lời đúng thì người trả lời đúng nhanh nhất nhận 3,34 điểm, hai người còn lại mỗi người nhận 3,33 điểm.

## 4. Luồng cuộc chơi

1. Người chơi đăng ký thông tin và tham gia cuộc chơi.
2. Máy chủ gửi câu hỏi chọn người chơi chính cho tất cả người tham gia; nếu không ai trả lời đúng, máy chủ gửi câu hỏi mới và lặp lại vòng chọn.
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

## 6. Các quy ước đã thống nhất

1. Hết thời gian mà chưa trả lời có được tính là trả lời sai hay không.
   - Có. Hết thời gian mà chưa trả lời được tính là trả lời sai và được xử lý theo luật của tình huống đó.
2. Cách xác định người nhanh nhất, xử lý thời điểm trả lời trùng nhau và độ trễ mạng.
   - Máy chủ căn cứ vào thời điểm nhận được câu trả lời hợp lệ. Nếu thời điểm nhận trùng nhau, thứ tự máy chủ tiếp nhận được dùng để phân định.
3. Cách chọn người chơi chính nếu không ai trả lời đúng câu hỏi mở đầu.
   - Máy chủ gửi câu hỏi mở đầu mới và lặp lại vòng chọn người chơi chính cho đến khi có người trả lời đúng. Vòng chọn lại không làm thay đổi điểm và không loại người chơi vì trả lời sai hoặc hết giờ.
4. Khi người chơi chính trả lời sai, người chơi phụ trả lời sai có bị loại hay không và những ai được tính là “còn lại” để chia điểm.
   - Người chơi chính và người chơi phụ trả lời sai hoặc hết giờ đều bị loại. Điểm của người chơi chính được chia đều cho những người chơi phụ trả lời đúng; người trả lời đúng nhanh nhất trở thành người chơi chính tiếp theo. Nếu không có người chơi phụ trả lời đúng, cuộc chơi kết thúc mà không có người chiến thắng.
5. Khi bỏ qua câu hỏi, cách xử lý nếu không có người chơi phụ trả lời đúng và trạng thái của những người chơi phụ trả lời sai.
   - Nếu có người chơi phụ trả lời đúng, người chơi chính giữ lại một nửa điểm; nửa điểm còn lại cùng với tổng điểm của người trả lời sai được chia đều cho những người trả lời đúng. Người chơi phụ trả lời sai hoặc hết giờ bị loại. Nếu không có ai trả lời đúng, không chuyển điểm; người chơi chính giữ nguyên điểm và chiến thắng vì là người duy nhất còn lại. Người chơi phụ trả lời sai hoặc hết giờ bị loại.
6. Hai quyền bỏ qua được tính theo từng người chơi hay theo mỗi lần đảm nhận vai trò người chơi chính.
   - Mỗi người chơi có tổng cộng 2 lượt bỏ qua trong một cuộc chơi, chỉ dùng được khi đang là người chơi chính. Số lượt đã dùng được giữ nguyên qua các lần đảm nhận vai trò.
7. Thời điểm được yêu cầu bỏ qua và cách xử lý câu trả lời đã gửi khi chuyển câu hỏi.
   - Người chơi chính có thể gửi yêu cầu bỏ qua trong 60 giây trả lời của mình. Lựa chọn hợp lệ đầu tiên của người chơi chính (trả lời hoặc bỏ qua) được ghi nhận và không thể đổi sau đó. Máy chủ vẫn chờ hết 30 giây của người chơi phụ để chốt câu trả lời và tính điểm. Nếu người chơi chính gửi câu trả lời hoặc yêu cầu bỏ qua sớm, máy chủ ghi nhận lựa chọn đó nhưng chỉ chốt kết quả sau khi hết thời gian của người chơi phụ. Câu hỏi mới bắt đầu lượt thời gian mới: 30 giây cho người chơi phụ và 60 giây cho người chơi chính.
8. Cách biểu diễn điểm lẻ, làm tròn và xử lý phần dư khi chia điểm.
   - Điểm được lưu đến 2 chữ số thập phân và tính theo đơn vị `0,01`. Phần dư khi chia được phát từng `0,01` cho người nhận theo thứ tự trả lời đúng nhanh nhất. Khi bỏ qua, nếu `100 * P` là số lẻ, điểm người chơi chính giữ lại được làm tròn xuống đến `0,01`; phần còn lại được đưa vào quỹ chia.
9. Cách xử lý khi người chơi mất kết nối hoặc rời cuộc chơi.
   - Trong lượt chơi, người mất kết nối hoặc rời cuộc chơi được xem như không trả lời/hết giờ và bị xử lý như trả lời sai theo luật của tình huống đó.

## 7. Trạng thái dự án

- Làm rõ luật chơi và cập nhật README.md (13/11/2026)