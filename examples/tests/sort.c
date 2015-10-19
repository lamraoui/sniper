
// sort
void main()
{
    
    // ARGS
    int numItems = 10;
    int a[numItems];
    
    
    int beforeChecksum = 0;
    for (int i = 0; i < numItems; i++)
    {
        beforeChecksum += a[i];    
    }
    
    /* sort algorithm here */
    
    for (int i = 1; i < numItems; i++)
    {
        assert(a[i - 1] <= a[i]);
    }
    
    int afterChecksum = 0;
    for (int i = 0; i < numItems; i++)
    {
        afterChecksum += a[i];    
    }
    assert(afterChecksum == beforeChecksum);
}
