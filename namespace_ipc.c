int container_main(void* arg)
{
    printf("Container - inside the container!\n");
    sethostname("container",10); /* 设置hostname */
    execv(container_args[0], container_args);
    printf("Something's wrong!\n");
    return 1;
}
 
int main()
{
    printf("Parent - start a container!\n");
    int container_pid = clone(container_main, container_stack+STACK_SIZE, 
            CLONE_NEWUTS | CLONE_NEWIPC | SIGCHLD, NULL); /*启用CLONE_NEWIPC Namespace隔离 */
    waitpid(container_pid, NULL, 0);
    printf("Parent - container stopped!\n");
    return 0;
}