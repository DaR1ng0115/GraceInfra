# CH01-TENSOR

**Language**  
[简体中文](README.zh.md)

## INTRODUCTION
GraceInfra is a deep learning training and inference framework built from scratch with a focus on *teaching*. The entire project is developed using C++ and CUDA, so a certain foundation in C++ is required.
This project emphasizes high performance while using deep learning frameworks as a supplement, with the goal of learning low-level thinking (the author is also continuously learning :)).  
Therefore, if you only want to understand *automatic differentiation*, *computational graphs*, or *Transformer principles*, you may want to choose other excellent projects, such as karpathy/micrograd or tinygrad.  
If you want to *train a large model using C++*, *understand what is happening inside PyTorch*, and *learn how data structures, computer organization, and computer architecture are applied in programming*, then this project may be a good choice.
At the same time, this project covers a broad range of topics. If you are a junior or senior undergraduate student, a second-year graduate student, or primarily focused on finding a job, you may want to take a look at AIInfraGuide, which requires less time to study.  
Therefore, this project is especially recommended for people who are *genuinely interested* in the subject.
Since I am a student, the project will inevitably contain bugs, omissions, inaccuracies, and even mistakes. I hope everyone will be understanding, and I welcome criticism and corrections.  

In this chapter, we focus on:
- Engineering specifications
- Introduction to deep learning
- Linear algebra and deep learning

The goal of this chapter is to implement a basic Tensor data structure from scratch and understand how it is used in deep learning.

Before learning, you should have a basic understanding of:
- Linear algebra, especially matrices
- C/C++ fundamentals, including data types, pointers, functions, structures, and classes

## Engineering Specifications
**To do a good job, one must first sharpen one's tools.**  
First, you need to understand the project structure and the files, modules, and functions that may be needed.
A common convention is to place source files (`.cpp`/`.cxx`) in the `src` directory and header files (`.h`/`.hpp`) in the `include` directory.
The `src` and `include` directories may be further organized by function, such as `utils` for tools and `config` for configuration files. Create such subdirectories as needed; this project aims to maintain a clear and standardized file organization.
At the beginning of each file, you may also follow this pattern:
```cpp
//
// filename.cpp
// ProjectName
//
// Created by username on xx-xx-xx
```

## Project Conventions and Principles
The following principles guide this project:
- **Project principles:** First principles — “Return to the most basic axioms and facts of a subject, and reconstruct the inference process from scratch”; Occam's razor — “Do not multiply entities without necessity.”
- **Explicit over implicit:** The project should present its underlying logic through clear, understandable structures and code whenever possible, rather than hiding principles behind clever tricks.
- **Straightforward comments:** Avoid piling up terminology whenever possible. Prefer direct explanations and genuine reasoning. Keep in mind, however, that this may come at the cost of some formal rigor.

## Guide
Next, you can open [matrix.h](/include/matrix.h) to begin your first lesson. If you encounter difficulties while studying, asking AI for help is recommended, but please make sure you can reproduce the result without relying on AI. If you know how to use agents such as Claude Code or Codex, you can clone this project locally and enter the following prompt:

> Please help me analyze this project and tell me: 1. How should I learn this project? 2. What is the structure of this project? 3. What kind of people is this project suitable for learning?
