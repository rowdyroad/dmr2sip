'use strict';

function ImageUploadController(url, $scope, Upload)
{
    $scope.beforeSave = function()
    {
        for (var i in $scope.current.images) {
            $scope.current.images[i].order = i;
        }
        return true;
    }

    function addImage(image)
    {
        if (!$scope.current.images) {
            $scope.current.images = [];
        }
        $scope.current.images.push(image);
        return $scope.current.images[$scope.current.images.length - 1];
    }

    $scope.uploadFiles = function (files)
    {
        if (!files) {
            return;
        }
        var reader = new FileReader;
        var index = 0;

        reader.onload = function(res)
        {
            var file = files[index];
            var image = addImage({data: res.target.result, file: file});
            $scope.$apply();
            Upload.upload({
                  url: url,
                  data: {
                      images: [ file ]
                  },
                  ignoreLoadingBar:true
              }).then(function (response) {
                  image.filename = response.data[0].filename;
                  delete image.data;
                  delete image.file;
                  delete image.progress;
              }, function (response) {
                  if (response.status > 0) {
                      $scope.errorMsg = response.status + ': ' + response.data;
                  }
              }, function (evt) {
                image.progress =
                      Math.min(100, parseInt(100.0 * evt.loaded / evt.total));
              });
            if (++index < files.length) {
               reader.readAsDataURL(files[index]);
            }
        }
        reader.readAsDataURL(files[index]);
    }

    $scope.leftImage = function(index)
    {
        if (index > 0) {
            var image = $scope.current.images[index];
            $scope.current.images[index] = $scope.current.images[index  - 1];
            $scope.current.images[index - 1] = image;
        }
    }

    $scope.rightImage = function(index)
    {
        if (index < $scope.current.images.length) {
            var image = $scope.current.images[index];
            $scope.current.images[index] = $scope.current.images[index + 1];
            $scope.current.images[index + 1] = image;
        }
    }

    $scope.removeImage = function(index)
    {
        $scope.current.images.splice(index,1);
    }
}