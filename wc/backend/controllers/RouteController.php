<?php
namespace app\controllers;
use yii\rest\ActiveController;
use Yii;
class RouteController extends ActiveController
{
    public $serializer = [
        'class' => 'yii\rest\Serializer',
        'collectionEnvelope' => 'data',
    ];
    public $modelClass = 'app\models\Route';
}