'use strict'

function BaseDeleteFormController(prefix, pkAttribute, $scope, $http, $modal)
{
        $scope.delete = function(item)
        {

            var model = item || $scope.current;
            var modalInstance = $modal.open({
                templateUrl: '/views/deletedialog.html',
                controller: function ($scope,$http, $uibModalInstance, config)
                {
                    $scope.config = config;
                    $scope.confirm = function()
                    {
                        $uibModalInstance.close();
                    }

                    $scope.cancel = function()
                    {
                        $uibModalInstance.dismiss('cancel');
                    }
                },
                resolve: {
                    config: function() {  return {
                        title: 'Удаление элемента',
                        text: 'Вы уверены, что хотите удалить текущий элемент?',
                        confirm_btn_class:'btn-danger',
                        confirm_btn_text: 'Удалить'
                    } }
                }
            });

            modalInstance.result.then(function () {
                $http.delete(prefix + '/' + model[$scope.pkAttribute]).success(function() {
                    $scope.$emit('after_delete', model);
                });
            });
        }
}


