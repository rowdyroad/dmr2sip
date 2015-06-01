<?php
namespace app\controllers;
use yii\rest\ActiveController;
use Yii;
class EventController extends ActiveController
{
    public $serializer = [
        'class' => 'yii\rest\Serializer',
        'collectionEnvelope' => 'data',
    ];
    public $modelClass = 'app\models\Event';
}