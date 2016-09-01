<?php

namespace app\controllers;

use Yii;
use yii\web\Controller;

class StateController extends Controller
{
    public function actionStates()
    {
        return [
                "service"=>\Yii::$app->service->isRunning(),
                "points"=>\app\models\Point::find()->all()
        ];
    }

    public function actionRestart()
    {
        if (!\Yii::$app->service->restart()) {
            throw new \yii\web\ServerErrorHttpException("Restart failure", 510);
        }
    }

    public function actionLog()
    {
        return ["data"=>\Yii::$app->service->log()];
    }
}
