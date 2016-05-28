angular.module("traffic3App")
  .controller("serverCtrl", function ($scope, $http, $location, $route, $interval) {
    $scope.data = {};
    $scope.addActive = 0;
    $scope.protocols = {};
    $scope.contentmanagers = {};

    $scope.add = {
      id: 0,
      host: "localhost",
      port: 10000,
      min: 100,
      max: 1000
    };
    $http.get("/status/supports/protocol").success(function (data) {
      $scope.protocols = data;
    }).error(function (error) {
      $scope.error = error;
    });


    $http.get("/status/supports/contentmanager").success(function (data) {
      $scope.contentmanagers = data;
    }).error(function (error) {
      $scope.error = error;
    });


    $scope.updateStatus = function() {
      $http.get("/server").success(function (data) {
        $scope.data = data;
      }).error(function (error) {
        $scope.error = error;
      });
    };
    $scope.updateStatus();


    $scope.interval = $interval(function() {
      $scope.updateStatus();
      console.log("UpdateServerStatus");
    }, 5000);

    $scope.showAdd = function() {
      $scope.add.error = null;
      $scope.addActive = 1;
    }

    $scope.hideAdd = function() {
      $scope.addActive = 0;
    }

    $scope.isAddActive = function() {
      return $scope.addActive == 1;
    }

    $scope.wasAddSuccess = function() {
      return angular.isUndefined($scope.add.error) || $scope.add.error == null;
    }

    $scope.addSubmit = function() {
      $http.put("/server?id=" + $scope.add.id +
                        "&protocol=" + $scope.add.protocol +
                        "&contentmanager=" + $scope.add.contentmanager +
                        "&host=" + $scope.add.host +
                        "&port=" + $scope.add.port +
                        "&min=" + $scope.add.min +
                        "&max=" + $scope.add.max
                      ).success(function (data) {
                        if (data.result == "Ok") {
                          $scope.add.error = null;
                          $scope.hideAdd();
                          $route.reload();
                        } else {
                          $scope.add.error = "Add failed";
                        }
                      }).error(function (error) {
                        console.log("Add failed: " + error);
                        $scope.add.error = error;
                      });
    }

    $scope.deleteServer = function(id) {
      $http.delete("/server?id=" + id).success(function (data) {
        $route.reload();
      }).error(function (error) {
        console.log(error);
      });
    }

    $scope.getNumRead = function(contentManager) {
      returnValue = 0;
      angular.forEach(contentManager, function(item) {
        returnValue += item.protocol.numRead;
      });
      return returnValue;
    };

    $scope.getNumWritten = function(contentManager) {
      returnValue = 0;
      angular.forEach(contentManager, function(item) {
        returnValue += item.protocol.numWritten;
      });
      return returnValue;
    };
  });
