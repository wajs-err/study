import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize_scalar
from scipy.optimize import bracket
np.random.seed(214)

func_name = 'quadratic'

if func_name == 'rosenbrock':
    def rosenbrock_function(x):
        return 10 * (x[1] - x[0]**2)**2 + (x[0] - 1)**2

    def grad_rosenbrock(x):
        return np.array([40 * x[0]**3 + (2 - 40 * x[1]) * x[0] - 2, 20 * (x[1] - x[0]**2)]) 

    f = rosenbrock_function
    gradf = grad_rosenbrock
    max_iter = 1000
    c_1 = 1e-4 
    c_2 = 0.99
    rho = 0.75
    epsilon = np.sqrt(np.finfo(float).eps)
    alpha = 0.06
    beta = 0.1
    fixed_step = 1e-3
    optimal_point = np.array([1.0, 1.0])
elif func_name == 'quadratic':
    cond_number = 30
    M = np.random.randn(2, 2)
    M = np.dot(M, M.T)
    U, s, V = np.linalg.svd(M)
    s = np.linspace(cond_number, 1, len(s))
    A = np.dot(U, np.dot(np.diag(s), V))
    b = np.random.randn(2)

    def quadratic_function(x):
        return 0.5 * np.dot(x.T, np.dot(A, x)) - np.dot(b.T, x)

    def grad_quadratic(x):
        return np.dot(A, x) - b

    f = quadratic_function
    gradf = grad_quadratic
    max_iter = 100
    c_1 = 1e-4
    c_2 = 0.9
    rho = 0.75
    epsilon = np.sqrt(np.finfo(float).eps)
    alpha = 0.3
    beta = 0.8
    fixed_step = 5e-2
    optimal_point = np.linalg.solve(A, b)


def gradient_descent(start_point, stepsize_func, max_iter=100):
    x = start_point.copy()
    funcalls = 1
    trajectory = [x.copy()]
    values = [f(x)]
    funcalls_res = [funcalls]
    for _ in range(max_iter):
        grad = gradf(x)
        step_size, funcalls_new = stepsize_func(x, grad)
        funcalls += funcalls_new + 1
        x -= step_size * grad
        trajectory.append(x.copy())
        values.append(f(x))
        funcalls_res.append(funcalls)
    return np.array(trajectory), np.array(values), funcalls_res


def backtracking_line_search(x, grad, alpha, beta, method='default'):
    def objective(t):
        return f(x - t * grad)

    funcalls = 0

    def sufficient_decrease_condition():
        nonlocal funcalls 
        funcalls += 2
        return objective(alpha) <= f(x) - c_1 * alpha * grad.T @ grad 

    def curvature_condition():
        nonlocal funcalls 
        funcalls += 0
        return gradf(x - alpha * grad).T @ grad <= c_2 * grad.T @ grad 
   
    def wolfe_condition():
        # funcalls increased in corresponding conditions 
        return sufficient_decrease_condition() and curvature_condition()

    def goldstein_condition():
        nonlocal funcalls 
        funcalls += 2
        return objective(alpha) >= f(x) - rho * alpha * grad.T @ grad \
           and objective(alpha) <= f(x) - (1 - rho) * alpha * grad.T @ grad

    def dichotomy():
        a, _, b, _, _, _, funcalls = bracket(objective)
        c = (a + b) / 2
        while abs(b - a) > epsilon:
            y = (a + c) / 2
            if objective(y) <= objective(c):
                funcalls += 2
                b, c = c, y 
            else:
                funcalls += 1
                z = (b + c) / 2
                if objective(c) <= objective(z):
                    a, b = y, z
                else:
                    a, c = c, z
        return c, funcalls

    if method == 'golden':
        res = minimize_scalar(objective, method='golden')
        return res.x, res.nfev
    elif method == 'dichotomy':
        return dichotomy() 
    elif method == 'wolfe':
        condition = wolfe_condition
    elif method == 'curvature':
        condition = curvature_condition
    elif method == 'goldstein':
        condition = goldstein_condition
    elif method == 'sufficient_decrease':
        condition = sufficient_decrease_condition
    else:
        raise Exception('unsupported method')

    while not condition():
        alpha *= beta
    return alpha, funcalls

start_point = np.array([-1.0, 2.0])

fixed = gradient_descent(start_point, lambda x, g: (fixed_step, 0), max_iter)
golden = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='golden'), max_iter)
goldstein = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='goldstein'), max_iter)
wolfe = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='wolfe'), max_iter)
sufficient_decrease = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='sufficient_decrease'), max_iter)
dichotomy = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='dichotomy'), max_iter)
curvature = gradient_descent(start_point, lambda x, g: backtracking_line_search(x, g, alpha, beta, method='curvature'), max_iter)

x1, x2 = np.meshgrid(np.linspace(-2.5, 2.5, 400), np.linspace(-2.5, 2.5, 400))
Z = np.array([f(np.array([x, y])) for x, y in zip(x1.flatten(), x2.flatten())]).reshape(x1.shape)

plt.figure(figsize=(10, 8))
plt.contour(x1, x2, Z, levels=50, cmap='viridis')
plt.plot(sufficient_decrease[0][:, 0], sufficient_decrease[0][:, 1], 'o-', label='Sufficient Decrease Condition')
plt.plot(wolfe[0][:, 0], wolfe[0][:, 1], 'o-', label='Wolfe Condition')
plt.plot(fixed[0][:, 0], fixed[0][:, 1], 'o-', label='Fixed Step Size')
plt.plot(goldstein[0][:, 0], goldstein[0][:, 1], 'o-', label='Goldstein Condition')
plt.plot(dichotomy[0][:, 0], dichotomy[0][:, 1], 'o-', label='Binary Search')
plt.plot(golden[0][:, 0], golden[0][:, 1], 'o-', label='Golden Search')
# don't plot curvature condition, because method doesn't converge with such hyperparameters
# plt.plot(curvature[0][:, 0], curvature[0][:, 1], 'o-', label='Curvature Condition')

plt.plot(start_point[0], start_point[1], 'ro', label='Start Point')
plt.plot(optimal_point[0], optimal_point[1], 'y*', markersize=15, label='Optimal Point')

plt.legend()
plt.title(f"Gradient Descent Trajectories on {'Quadratic' if func_name == 'quadratic' else 'Rosenbrock'} Function")
plt.xlabel('x1')
plt.ylabel('x2')
plt.savefig(f'{func_name}.png', dpi=300)

plt.figure(figsize=(10, 8))
plt.plot(sufficient_decrease[2], sufficient_decrease[1], '.-', label='Sufficient Decrease Condition')
plt.plot(wolfe[2], wolfe[1], '.-', label='Wolfe Condition')
plt.plot(fixed[2], fixed[1], '.-', label='Fixed Step Size')
plt.plot(goldstein[2], goldstein[1], '.-', label='Goldstein Condition')
plt.plot(dichotomy[2], dichotomy[1], '.-', label='Binary Search')
plt.plot(golden[2], golden[1], '.-', label='Golden Search')
# don't plot curvature condition, because method doesn't converge with such hyperparameters
# plt.plot(curvature[2], curvature[1], '.-', label='Curvature Condition')
plt.xlim(0, 250)
plt.legend()
plt.title(f"Efficiencies Of Different Methods On {'Quadratic' if func_name == 'quadratic' else 'Rosenbrock'} Function")
plt.xlabel('Number Of Function Evaluations')
plt.ylabel('Function Value')
plt.savefig(f'{func_name}_efficiencies.png', dpi=300)
