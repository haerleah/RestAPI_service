plugins {
	java
	id("org.springframework.boot") version "3.4.4"
	id("io.spring.dependency-management") version "1.1.7"
}

group = "ru.s21.server"
version = "0.0.1-SNAPSHOT"

java {
	toolchain {
		languageVersion = JavaLanguageVersion.of(21)
	}
}

configurations {
	compileOnly {
		extendsFrom(configurations.annotationProcessor.get())
	}
}

repositories {
	mavenCentral()
}

dependencies {
	implementation("ru.s21.lib:race:1.0")
	implementation("net.java.dev.jna:jna:5.17.0")
	implementation("net.java.dev.jna:jna-platform:5.17.0")
	implementation("org.springframework.boot:spring-boot-starter-web")
    implementation("org.springframework.boot:spring-boot-starter-actuator")
    compileOnly("org.projectlombok:lombok")
	annotationProcessor("org.projectlombok:lombok")
	testImplementation("org.springframework.boot:spring-boot-starter-test")
	testRuntimeOnly("org.junit.platform:junit-platform-launcher")
}

tasks.withType<Test> {
	useJUnitPlatform()
}

tasks.bootRun {
	dependsOn("copyLibraries")
}

tasks.named("processResources") {
	mustRunAfter("copyLibraries")
}

tasks.register<Exec>("compileSnakeLibrary") {
	commandLine("make", "-C", layout.projectDirectory.dir("../snake"))
}

tasks.register<Exec>("compileTetrisLibrary") {
	commandLine("make", "-C", layout.projectDirectory.dir("../tetris"))
}

tasks.register<Copy>("copyLibraries") {
	dependsOn("compileSnakeLibrary")
	dependsOn("compileTetrisLibrary")
	from(listOf("../snake", "../tetris")){
		include("*.so")
	}
	into("src/main/resources")
}
