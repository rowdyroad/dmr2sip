<?php

namespace app\controllers;

use Yii;
use yii\web\Controller;

class CommutatorController extends Controller
{
    const SERVICE_NAME  = 'commutator';

    public function actionState()
    {
        return [ 'active' => strpos(exec(sprintf("service %s status", self::SERVICE_NAME)), "is running") !== false ];
    }
}
