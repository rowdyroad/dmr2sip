<?php

$params = require(__DIR__ . '/params.php');

$config = [
    'id' => 'basic',
    'basePath' => dirname(__DIR__),
    'bootstrap' => ['log',[

            'class' => 'yii\filters\ContentNegotiator',
            'formats' => [
                'application/json' => \yii\web\Response::FORMAT_JSON,
                'application/xml' =>  \yii\web\Response::FORMAT_JSON
            ],

    ]],
    'components' => [
        'request' => [
            // !!! insert a secret key in the following (if it is empty) - this is required by cookie validation
            'cookieValidationKey' => 'E5TZYQ7aOX5mYSr7ErHe0f88lFrPWP-l',
	    'enableCsrfValidation'=>false,
            'parsers' => [
               'application/json' => '\yii\web\JsonParser',
            ]
        ],
        'user' => [
            'identityClass' => 'app\models\User',
            'enableAutoLogin' => true,
        ],
        'errorHandler' => [
            'errorAction' => 'site/error',
        ],
        'mailer' => [
            'class' => 'yii\swiftmailer\Mailer',
            // send all mails to a file by default. You have to set
            // 'useFileTransport' to false and configure a transport
            // for the mailer to send real emails.
            'useFileTransport' => true,
        ],
        'log' => [
            'traceLevel' => YII_DEBUG ? 3 : 0,
            'targets' => [
                [
                    'class' => 'yii\log\FileTarget',
                    'levels' => ['error', 'warning'],
                ],
            ],
        ],
        'response' => [
            'format' => yii\web\Response::FORMAT_JSON,
            'charset' => 'UTF-8',
        ],
        'urlManager' => [
            'enablePrettyUrl' => true,
            'enableStrictParsing' => true,
            'showScriptName' => false,
            'rules' => [
                [
                    'class' => 'yii\rest\UrlRule',
                    'controller' => [
                                        'event','user','point','route'
                                    ],
                    'tokens'=> [ '{id}' => '<id:.+>' ]
                ],
                ['class' => 'yii\web\UrlRule', 'route'=>'configuration/network', 'pattern'=>'configuration/network', 'verb'=>['GET','POST']],
                ['class' => 'yii\web\UrlRule', 'route'=>'commutator/state', 'pattern'=>'commutator/state', 'verb'=>'GET'],
                ['class' => 'yii\web\UrlRule', 'route'=>'commutator/reload', 'pattern'=>'commutator/reload', 'verb'=>'POST'],
                ['class' => 'yii\web\UrlRule', 'route'=>'user/check', 'pattern'=>'user/check'],
                ['class' => 'yii\web\UrlRule', 'route'=>'user/login', 'pattern'=>'user/login','verb'=>'POST'],
                ['class' => 'yii\web\UrlRule', 'route'=>'user/logout', 'pattern'=>'user/logout', 'verb'=>'POST'],
            ],
        ],
        'db' => require(__DIR__ . '/db.php'),
    ],
    'params' => $params,
];

if (YII_ENV_DEV) {
    // configuration adjustments for 'dev' environment
    $config['bootstrap'][] = 'debug';
    $config['modules']['debug'] = 'yii\debug\Module';

    $config['bootstrap'][] = 'gii';
    $config['modules']['gii'] = 'yii\gii\Module';
}

return $config;
