/**
 * Created by naoki on 2018/07/09.
 */
(function () {
    var scene;
    var sphere;
    var camera;
    var light;
    var renderer;
    var width = 500;
    var height = 250;

    // scene ステージ
    scene = new THREE.Scene();

    // mesh 物体
    sphere = new THREE.Mesh(
        new THREE.SphereGeometry(100, 20, 20), // geometry 形状
        new THREE.MeshLambertMaterial({color: 0x8dc3ff}) // material 材質、色
    );
    sphere.position.set(0, 0, 0);
    scene.add(sphere);

    // light
    light = new THREE.DirectionalLight(0xffffff, 1);
    light.position.set(0, 100, 30);
    scene.add(light);

    // camera
    camera = new THREE.PerspectiveCamera(45, width / height, 1, 1000);
    camera.position.set(200, 100, 300);
    camera.lookAt(scene.position);

    // renderer
    renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setSize(width, height);
    renderer.setClearColor(0xeeeeee);
    document.getElementById('stage').appendChild(renderer.domElement);
    renderer.render(scene, camera);
})();