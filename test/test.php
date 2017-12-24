<?php

//TEST 1
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page=1");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

$dataArr = json_decode($dataRaw,true);

print_r($dataArr); 


//TEST 2
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);

//TEST 3
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);

//TEST 4
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);

//TEST 5
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);


//TEST 6
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);


//TEST 6
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page=");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);


//TEST 7
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page=0");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);


//TEST 8
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=10&query_page=1");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

$dataArr = json_decode($dataRaw,true);

print_r($dataArr); 

//TEST 9
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=百度&query_number=10&query_page=1");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

$dataArr = json_decode($dataRaw,true);

print_r($dataArr); 


//TEST 10
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number=&query_page=0");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);

//TEST 11
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=科技&query_number&query_page=0");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

print_r($dataRaw);


//TEST 12
$startTs = microtime();

$dataRaw = file_get_contents("http://127.0.0.1:9003/?query_word=百度&query_number=10&query_page=1");

$endTs = microtime();

printf("cost time:" . ($endTs-$startTs)*1000 . "ms" . PHP_EOL);

$dataArr = json_decode($dataRaw,true);

print_r($dataArr); 
