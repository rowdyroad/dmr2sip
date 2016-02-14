<?php

namespace app\controllers;

use Yii;
use yii\web\Controller;

class CommutatorController extends Controller
{
    const SERVICE_NAME  = 'commutator';

    private function serviceCommand($command)
    {
        return exec(sprintf("service %s %s", self::SERVICE_NAME, $command));
    }

    public function actionState()
    {
        return [ 'active' => strpos($this->serviceCommand('status'), "is running") !== false ];
    }

    public function actionReload()
    {
        return [ 'success' => strpos($this->serviceCommand('reload'), "done") !== false ];
    }
}
