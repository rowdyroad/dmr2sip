<?php
namespace app\commands;
use yii\console\Controller;
use app\models\Event;
use Yii;

class EventController extends Controller
{
    public function actionIndex($route_id, $source_number)
    {
	$event = new Event;
	$event->route_id = $route_id;
	$event->source_number = $source_number;
	$event->save();
    }
}
