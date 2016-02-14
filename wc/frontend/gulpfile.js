var gulp = require('gulp'),
	mainBowerFiles = require('main-bower-files'),
	inject = require('gulp-inject'),
	readFile = require('fs').readFileSync,
	rimraf = require('gulp-rimraf'),
	templateCache = require('gulp-angular-templatecache'),
	sass = require('gulp-sass'),
	concat = require('gulp-concat'),
	concatCss = require('gulp-concat-css'),
	md5 = require('gulp-md5'),
	sort = require('gulp-sort');

var bowerDirectory = process.cwd() + '/' + JSON.parse(readFile('.bowerrc', 'utf-8')).directory;
var public_dir = 'public';
var tmp_dir= 'tmp';

gulp.task("clean-tmp-lib", function() {
	return gulp.src(tmp_dir + '/lib', {read:false})
			.pipe(rimraf());
});

gulp.task("clean-tmp-app", function() {
	return gulp.src(tmp_dir + '/app', {read:false})
			.pipe(rimraf());
});


gulp.task('collect-lib-js', function() {
 	return gulp.src(mainBowerFiles({filter:/\.js$/}), { base: bowerDirectory })
			.pipe(concat('lib.js'))
			.pipe(md5())
			.pipe(gulp.dest(tmp_dir + '/lib'));
});

gulp.task('collect-lib-css', function() {
 	return gulp.src(mainBowerFiles({filter:/\.css$/}), { base: bowerDirectory })
			.pipe(concatCss('lib.css'))
			.pipe(md5())
			.pipe(gulp.dest(tmp_dir + '/lib'));
});

gulp.task('collect-lib-others', function() {
 	return gulp.src(mainBowerFiles({
 					filter: function(file) {
 						return 	file.indexOf('.css', file.length - 4) == -1
 								&& file.indexOf('.js', file.length - 3) == -1;
 					}
 		}), { base: bowerDirectory }) .pipe(gulp.dest(tmp_dir + '/lib'));
});

gulp.task('collect-lib', ['collect-lib-js','collect-lib-css', 'collect-lib-others']);

gulp.task('collect-app-templates', function() {
	return gulp.src(['src/app/views/**/*.html'])
		.pipe(templateCache({root: '/views', module:'ac', standalone:'[]'}))
    	.pipe(gulp.dest(tmp_dir + '/app'));
});

gulp.task('collect-app-css', function() {
   	return gulp.src(['src/styles/**/*', '!src/styles/**/*.scss'], {base:'src/styles'})
            .pipe(gulp.dest(tmp_dir + '/css'));
});

gulp.task('collect-app-sass', function() {
    var sassOptions = {
        outputStyle: 'nested'
    };
    return gulp
        .src('src/styles/**/*.scss')
        .pipe(sass(sassOptions))
        .pipe(gulp.dest(tmp_dir + '/css'));
});

gulp.task('collect-app-js', function() {
	return gulp.src(['src/app/**/*.js', "!src/app/app.js"], {base: 'src'})
			.pipe(concat("app_files.js"))
			.pipe(gulp.dest(tmp_dir + "/app"));
});

gulp.task("collect-app", ['collect-app-templates','collect-app-js','collect-app-css', 'collect-app-sass'], function() {
   return gulp.src('src/app/app.js', {base: 'src'})
        	.pipe(gulp.dest(tmp_dir));
});

gulp.task("clean-public", function() {
	return gulp.src(public_dir, {read:false})
			.pipe(rimraf());
});

gulp.task('publish-static',["clean-public"], function() {
	return gulp.src(['src/i18n/**/*','src/images/**/*'], {base:'src'})
         	.pipe(gulp.dest(public_dir));
});

gulp.task('publish-lib', ["collect-lib","clean-public"], function() {
	return gulp.src(tmp_dir + '/lib/**/*')
         	.pipe(gulp.dest(public_dir + '/lib'));
});

gulp.task('publish-css', ["collect-app","clean-public"], function() {
 	return gulp.src(tmp_dir + '/css/**/*.css')
    		.pipe(concat('app.css'))
    		.pipe(md5())
    		.pipe(gulp.dest(public_dir + '/app'));
});

gulp.task('publish-app', ["collect-app","clean-public"], function() {
	return gulp.src([tmp_dir + '/app/app_files.js', tmp_dir + '/app/templates.js', tmp_dir + '/app/app.js'])
		.pipe(concat('app.js'))
		.pipe(md5())
		.pipe(gulp.dest(public_dir + '/app'));
})

gulp.task("publish", ["publish-static", "publish-lib", "publish-css", "publish-app"]);

gulp.task("inject", ["publish"], function() {
	gulp.src(tmp_dir, {read:false}).pipe(rimraf());
	var app = gulp.src(public_dir +'/app/**/*', { read: false });
	var lib = gulp.src(public_dir +'/lib/**/*', { read: false });

	return gulp.src('src/index.html')
		.pipe(inject(lib, { name:'lib', ignorePath: public_dir, relative:false}))
		.pipe(inject(app, { name:'app', ignorePath: public_dir, relative:false}))
		.pipe(gulp.dest(public_dir));
});

gulp.task("default", ["inject"]);


