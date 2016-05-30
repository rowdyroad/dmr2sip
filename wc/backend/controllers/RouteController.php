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


    public function actions()
    {
        return array_diff_key(parent::actions(), ['index'=>null]);
    }

    public function actionIndex()
    {
        $model = $this->modelClass;
        return new \yii\data\ActiveDataProvider([
			'query'=>$model::find()->orderBy('`order` asc')
		]);
    }

}