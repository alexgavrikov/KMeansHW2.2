Всего 3 программы - главный сервер, клиент-раб, программа-тест (считает метрику на тестовых данных).
У каждой из 3 программ своя функция main, свой makefile и свои дополнительные файлы с кодом. Хотя есть общий код, выделенный
в отдельную папку Common.
Каждой программе выделена папка, где хранится её код  и makefile. 
Ещё есть папка out, куда я при тестировании сохранял файлы с результатами работ программ.

Для того, чтобы собрать программу-сервер надо в папке Server запустить:
	make -f ServerMakefile
Остальные программы аналогично.

Сначала запускается сервер, потом клиенты-рабы.

Запуск сервера:
	server_main clusters_number input_file output_file port_for_slaves slaves_num
Например:
	./server_main 10 shad.learn ../out/K=10_slaves_count=2_centroids.txt 1234 2

Запуск раба:
	slave_main port_to_bind
Например:
	./slave_main 1234

Запуск теста:
	test_main input_file_with_test_data input_file_with_centroids output_file
Например:
	/test_main shad.test ../out/K=10_slaves_count=2_centroids.txt ../out/K=10_slaves_count=2_RESULT.txt 



Формат вывода центроидов в файл:
 centroid_id:0 \t component \t component \t ... \t component
...
 centroid_id:K-1 \t component \t component \t ... \t component
 
Правила общения между клиентом и сервером:
Формат сообщения:
	content_length \t message
Например:
	3 \t OK

Само сообщение представляет из себя последовательность чисел, разделённых разделителями '#' и ','.
'#' перед каждой новой точкой (то есть перед dimensions компонентами, которых в датасете-примере 10).
',' после каждого числа, будь то компонента точки или что-нибудь ещё. Таким образом да, между точками будут рядом
стоять ',' и '#', ну и ладно.
