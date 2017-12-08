<?php
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page=1");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

$dataArr = json_decode($dataRaw,true);

print_r($dataArr); 
